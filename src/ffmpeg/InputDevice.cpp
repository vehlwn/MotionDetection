#include "InputDevice.hpp"

#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>

extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
#include <libavutil/rational.h>
}

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include "ffmpeg/ScopedAvDictionary.hpp"
#include "ffmpeg/detail/AVRationalOutput.hpp"
#include "ffmpeg/detail/AvError.hpp"
#include "ffmpeg/detail/AvFrameAdapters.hpp"
#include "ffmpeg/detail/OutputFile.hpp"
#include "ffmpeg/detail/ScopedAvFormatInput.hpp"
#include "ffmpeg/detail/ScopedDecoderContext.hpp"
#include "ffmpeg/detail/SwsPixelConverter.hpp"

namespace vehlwn::ffmpeg {
using DecoderContextsMap = std::map<int, detail::ScopedDecoderContext>;

struct InputDevice::Impl {
    detail::ScopedAvFormatInput input_format_context;
    DecoderContextsMap decoder_contexts;
    detail::SwsPixelConverter pixel_converter;
    std::optional<detail::OutputFile> output_file;

    std::queue<detail::OwningAvframe> video_frames_queue;

    std::optional<std::string> video_bitrate;
    std::optional<std::string> audio_bitrate;

    Impl(
        detail::ScopedAvFormatInput&& input_format_context_,
        DecoderContextsMap&& decoder_contexts_,
        detail::SwsPixelConverter&& pixel_converter_)
        : input_format_context(std::move(input_format_context_))
        , decoder_contexts(std::move(decoder_contexts_))
        , pixel_converter(std::move(pixel_converter_))
    {}

    detail::OwningAvPacket read_packet()
    {
        BOOST_LOG_FUNCTION();
        while(true) {
            detail::OwningAvPacket ret = input_format_context.read_packet();
            const int in_stream_index = ret.stream_index();
            const auto decoder = decoder_contexts.find(in_stream_index);
            // Ignoge all non video and non audio streams
            if(decoder != decoder_contexts.end()) {
                BOOST_LOG_TRIVIAL(trace)
                    << "packet: stream = " << in_stream_index
                    << " pts = " << ret.pts() << " dts = " << ret.dts();
                return ret;
            }
        }
    }

    void decode_packet_to_queue(const detail::OwningAvPacket& packet)
    {
        BOOST_LOG_FUNCTION();
        const int in_stream_index = packet.stream_index();
        auto& decoder_context = decoder_contexts.at(in_stream_index);
        decoder_context.send_packet(packet);
        const auto in_stream_timebase
            = input_format_context
                  .streams()[static_cast<std::size_t>(in_stream_index)]
                  ->time_base;
        while(true) {
            auto decoded_result = decoder_context.receive_frame();
            if(auto* const decoded_frame
               = std::get_if<detail::OwningAvframe>(&decoded_result)) {
                const double d_pts = (double)decoded_frame->best_effort_timestamp()
                    * av_q2d(in_stream_timebase);
                BOOST_LOG_TRIVIAL(trace)
                    << "frame: stream = " << in_stream_index
                    << " pts = " << decoded_frame->pts()
                    << " best_effort = " << decoded_frame->best_effort_timestamp()
                    << " d_pts = " << d_pts << " pict_type = "
                    << av_get_picture_type_char(decoded_frame->pict_type());
                decoded_frame->set_pts(decoded_frame->best_effort_timestamp());
                // Write frame to a file
                if(output_file) {
                    output_file.value().encode_write_frame(
                        *decoded_frame,
                        in_stream_index);
                }
                // Save it to queue
                if(decoder_context.codec_type() == AVMEDIA_TYPE_VIDEO) {
                    video_frames_queue.emplace(std::move(*decoded_frame));
                }
            } else if(std::holds_alternative<detail::ScopedDecoderContext::Again>(
                          decoded_result)) {
                break;
            } else {
                throw std::runtime_error("Unreachable!");
            }
        }
    }
};

InputDevice::InputDevice(std::unique_ptr<Impl>&& pimpl_)
    : pimpl(std::move(pimpl_))
{}

InputDevice::~InputDevice() = default;

InputDevice::InputDevice(InputDevice&&) noexcept = default;

InputDevice& InputDevice::operator=(InputDevice&&) noexcept = default;

CvMatRaiiAdapter InputDevice::get_video_frame() const
{
    while(pimpl->video_frames_queue.empty()) {
        const auto packet = pimpl->read_packet();
        pimpl->decode_packet_to_queue(packet);
    }
    auto next_frame = std::move(pimpl->video_frames_queue.front());
    pimpl->video_frames_queue.pop();
    auto ret = pimpl->pixel_converter.frame_to_cv_mat(next_frame);
    return CvMatRaiiAdapter(std::move(ret));
}

double InputDevice::fps() const
{
    const auto values = boost::adaptors::values(pimpl->decoder_contexts);
    const auto it = boost::find_if(values, [](const auto& x) {
        return x.codec_type() == AVMEDIA_TYPE_VIDEO;
    });
    if(it != values.end()) {
        return av_q2d(it->framerate());
    }
    return 0.0;
}

void InputDevice::start_recording(const char* const path) const
{
    pimpl->output_file.emplace(open_output_file(
        path,
        pimpl->decoder_contexts,
        pimpl->input_format_context.streams(),
        pimpl->video_bitrate,
        pimpl->audio_bitrate));
}

void InputDevice::stop_recording() const
{
    pimpl->output_file = std::nullopt;
}

bool InputDevice::is_recording() const
{
    return pimpl->output_file.has_value();
}

void InputDevice::set_out_video_bitrate(std::optional<std::string>&& x) const
{
    pimpl->video_bitrate = std::move(x);
}

void InputDevice::set_out_audio_bitrate(std::optional<std::string>&& x) const
{
    pimpl->audio_bitrate = std::move(x);
}

InputDevice open_input_device(
    const char* const url,
    const std::optional<std::string>& file_format,
    ScopedAvDictionary& options)
{
    BOOST_LOG_FUNCTION();
    {
        static const bool register_devices_flag = [] {
            avdevice_register_all();
            return true;
        }();
        (void)register_devices_flag;
    }

    BOOST_LOG_TRIVIAL(debug) << "Demuxer options = " << options;
    detail::ScopedAvFormatInput input_format_context(url, file_format, options);
    BOOST_LOG_TRIVIAL(debug) << "Unsupported options = " << options;
    input_format_context.find_stream_info();
    input_format_context.dump_format();

    DecoderContextsMap decoder_contexts;
    std::optional<detail::SwsPixelConverter> pixel_converter;
    int video_stream_index = -1;

    boost::for_each(
        boost::adaptors::index(input_format_context.streams()),
        [&](const auto& elem) {
            const auto stream_index = static_cast<int>(elem.index());
            AVStream* const local_stream = elem.value();

            const AVCodecParameters* const local_codec_par = local_stream->codecpar;
            const AVMediaType codec_type = local_codec_par->codec_type;
            if(codec_type != AVMEDIA_TYPE_VIDEO
               && codec_type != AVMEDIA_TYPE_AUDIO) {
                return;
            }

            const AVCodec* const local_decoder
                = avcodec_find_decoder(local_codec_par->codec_id);
            if(!local_decoder) {
                BOOST_LOG_TRIVIAL(error)
                    << "Failed to find decoder for stream #" << stream_index;
                throw std::runtime_error("Failed to find decoder");
            }
            detail::ScopedDecoderContext decoder_context(
                local_decoder,
                local_codec_par);
            switch(codec_type) {
            case AVMEDIA_TYPE_VIDEO: {
                decoder_context.guess_frame_rate(input_format_context, local_stream);
                if(video_stream_index == -1) {
                    pixel_converter = detail::SwsPixelConverter(
                        decoder_context.width(),
                        decoder_context.height(),
                        decoder_context.pix_fmt(),
                        AV_PIX_FMT_BGR24);
                    video_stream_index = stream_index;
                } else {
                    BOOST_LOG_TRIVIAL(warning)
                        << "Found another video stream: " << stream_index
                        << ". Using only " << video_stream_index << "-th";
                    return;
                }
                BOOST_LOG_TRIVIAL(debug)
                    << "VIDEO codec name = " << local_decoder->name << ", stream "
                    << stream_index;
                const AVRational framerate = decoder_context.framerate();
                BOOST_LOG_TRIVIAL(debug)
                    << "decoder: guess_frame_rate = " << framerate << " = "
                    << av_q2d(framerate)
                    << " time_base = " << decoder_context.time_base();
                break;
            }
            case AVMEDIA_TYPE_AUDIO: {
                if(decoder_context.ch_layout().order == AV_CHANNEL_ORDER_UNSPEC) {
                    av_channel_layout_default(
                        &decoder_context.ch_layout(),
                        decoder_context.ch_layout().nb_channels);
                }
                break;
            }
            default:
                throw std::runtime_error("Unreachable!");
            }
            // Set the packet timebase for the decoder.
            decoder_context.set_pkt_timebase(local_stream->time_base);
            BOOST_LOG_TRIVIAL(debug)
                << "stream " << stream_index << ":"
                << " timebase = " << local_stream->time_base
                << " r_frame_rate = " << local_stream->r_frame_rate
                << " start_time = " << local_stream->start_time;

            decoder_contexts.emplace(stream_index, std::move(decoder_context));
        });
    if(video_stream_index == -1 || !pixel_converter) {
        throw std::runtime_error("Input file does not contain video streams");
    }

    return InputDevice(std::make_unique<InputDevice::Impl>(
        std::move(input_format_context),
        std::move(decoder_contexts),
        std::move(pixel_converter.value())));
}
} // namespace vehlwn::ffmpeg