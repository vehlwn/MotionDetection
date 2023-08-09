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
#include <libavcodec/codec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
}

#include <boost/algorithm/string/join.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include "../SharedMutex.hpp"
#include "ScopedAvDictionary.hpp"
#include "detail/AVRationalOutput.hpp"
#include "detail/AvError.hpp"
#include "detail/AvFrameAdapters.hpp"
#include "detail/OutputFile.hpp"
#include "detail/ScopedAvFormatInput.hpp"
#include "detail/ScopedDecoderContext.hpp"
#include "detail/SwsPixelConverter.hpp"

namespace vehlwn::ffmpeg {
using DecoderContextsMap = std::map<int, detail::ScopedDecoderContext>;

struct InputDevice::Impl {
    detail::ScopedAvFormatInput input_format_context;
    DecoderContextsMap decoder_contexts;
    SharedMutex<std::optional<detail::OutputFile>> output_file;

    std::optional<detail::SwsPixelConverter> pixel_converter;
    std::queue<detail::OwningAvframe> video_frames_queue;

    std::optional<std::string> video_bitrate;
    std::optional<std::string> audio_bitrate;

    Impl(
        detail::ScopedAvFormatInput&& input_format_context_,
        DecoderContextsMap&& decoder_contexts_)
        : input_format_context(std::move(input_format_context_))
        , decoder_contexts(std::move(decoder_contexts_))
    {}

    detail::OwningAvPacket read_packet()
    {
        BOOST_LOG_FUNCTION();
        while(true) {
            detail::OwningAvPacket ret = input_format_context.read_packet();
            const int in_stream_index = ret.stream_index();
            // Ignoge all non video and non audio streams
            if(decoder_contexts.contains(in_stream_index)) {
                BOOST_LOG_TRIVIAL(trace)
                    << "packet: stream = " << in_stream_index
                    << " pts = " << ret.pts() << " dts = " << ret.dts();
                return ret;
            }
        }
    }

    void create_pixel_converter(const detail::OwningAvframe& input_frame)
    {
        pixel_converter.emplace(
            input_frame.width(),
            input_frame.height(),
            input_frame.format(),
            AV_PIX_FMT_BGR24);
    }

    void check_encode_write(
        const detail::OwningAvframe& frame,
        const int in_stream_index) const
    {
        // Write frame to a file
        const auto lock = output_file.write();
        auto& opt = *lock;
        if(opt) {
            opt->encode_write_frame(frame, in_stream_index);
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
                const auto d_pts
                    = static_cast<double>(decoded_frame->best_effort_timestamp())
                    * av_q2d(in_stream_timebase);
                BOOST_LOG_TRIVIAL(trace)
                    << "frame: stream = " << in_stream_index
                    << " pts = " << decoded_frame->pts()
                    << " best_effort = " << decoded_frame->best_effort_timestamp()
                    << " d_pts = " << d_pts << " pict_type = "
                    << av_get_picture_type_char(decoded_frame->pict_type());
                decoded_frame->set_pts(decoded_frame->best_effort_timestamp());

                // Save it to queue
                if(decoder_context.codec_type() == AVMEDIA_TYPE_VIDEO) {
                    if(!pixel_converter) {
                        // Cannot trust AVCodecContext::pix_fmt after
                        // decoder_context.open() because it can change after
                        // send_packet() when using hardware decoder.
                        create_pixel_converter(*decoded_frame);
                    }
                    auto converted_frame
                        = pixel_converter->scale_video(*decoded_frame);
                    converted_frame.set_pts(decoded_frame->pts());
                    check_encode_write(converted_frame, in_stream_index);
                    video_frames_queue.emplace(std::move(converted_frame));
                } else {
                    check_encode_write(*decoded_frame, in_stream_index);
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

    auto ret = next_frame.copy_to_cv_mat();
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
    const auto lock = pimpl->output_file.write();
    lock->emplace(open_output_file(
        path,
        pimpl->decoder_contexts,
        pimpl->input_format_context.streams(),
        pimpl->video_bitrate,
        pimpl->audio_bitrate));
}

void InputDevice::stop_recording() const
{
    const auto lock = pimpl->output_file.write();
    *lock = std::nullopt;
}

bool InputDevice::is_recording() const
{
    const auto lock = pimpl->output_file.read();
    return lock->has_value();
}

void InputDevice::set_out_video_bitrate(std::optional<std::string>&& x) const
{
    pimpl->video_bitrate = std::move(x);
}

void InputDevice::set_out_audio_bitrate(std::optional<std::string>&& x) const
{
    pimpl->audio_bitrate = std::move(x);
}

namespace {
void unique_register_all_ffmpeg_devices()
{
    static const bool register_devices_flag = [] {
        avdevice_register_all();
        return true;
    }();
    (void)register_devices_flag;
}

detail::ScopedAvFormatInput create_input_format_context(
    const char* const url,
    const std::optional<std::string>& file_format,
    ScopedAvDictionary& options)
{
    BOOST_LOG_TRIVIAL(debug) << "Demuxer options = " << options;
    auto ret = detail::ScopedAvFormatInput(url, file_format, options);
    if(options.size() != 0) {
        BOOST_LOG_TRIVIAL(error) << "Unsupported demuxer options: " << options;
        throw std::runtime_error("Found unsupported demuxer options");
    }
    ret.find_stream_info();
    ret.dump_format();
    return ret;
}

AVHWDeviceType find_hw_device_by_name(const char* const name)
{
    BOOST_LOG_FUNCTION();
    auto type = av_hwdevice_find_type_by_name(name);
    if(type == AV_HWDEVICE_TYPE_NONE) {
        const auto msg = std::string("Device type '") + name + "' is not supported";
        BOOST_LOG_TRIVIAL(error) << msg;

        auto avaliable_types = std::vector<std::string>();
        while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
            avaliable_types.emplace_back(av_hwdevice_get_type_name(type));
        }
        BOOST_LOG_TRIVIAL(error) << "Available device types: {"
                                 << boost::join(avaliable_types, ", ") << "}";
        throw std::runtime_error(msg);
    }
    return type;
}

AVPixelFormat
    find_hw_pix_fmt(const AVCodec* const decoder, const AVHWDeviceType type)
{
    BOOST_LOG_FUNCTION();
    auto hw_pix_fmt = AV_PIX_FMT_NONE;
    for(int i = 0;; i++) {
        const auto config = avcodec_get_hw_config(decoder, i);
        if(config == nullptr) {
            BOOST_LOG_TRIVIAL(error)
                << "Decoder " << decoder->name << " does not support device type "
                << av_hwdevice_get_type_name(type);
            throw std::runtime_error("Decoder does not support wanted device type");
        }
        if(((static_cast<unsigned>(config->methods)
             & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX)
            != 0)
           && config->device_type == type) {
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }
    return hw_pix_fmt;
}

DecoderContextsMap create_decoder_contexts(
    detail::ScopedAvFormatInput& input_format_context,
    const std::optional<std::string>& hw_decoder_type)
{
    BOOST_LOG_FUNCTION();
    auto ret = DecoderContextsMap();
    bool has_video_stream = false;
    for(auto&& [index, stream] :
        boost::adaptors::index(input_format_context.streams())) {
        const AVCodecParameters* const local_codec_par = stream->codecpar;
        const AVMediaType codec_type = local_codec_par->codec_type;
        if(codec_type != AVMEDIA_TYPE_VIDEO && codec_type != AVMEDIA_TYPE_AUDIO) {
            continue;
        }

        const auto local_decoder = avcodec_find_decoder(local_codec_par->codec_id);
        if(local_decoder == nullptr) {
            BOOST_LOG_TRIVIAL(error)
                << "Failed to find decoder for stream #" << index;
            throw std::runtime_error("Failed to find decoder");
        }
        auto decoder_context
            = detail::ScopedDecoderContext(local_decoder, local_codec_par);
        switch(codec_type) {
            case AVMEDIA_TYPE_VIDEO: {
                if(has_video_stream) {
                    BOOST_LOG_TRIVIAL(warning)
                        << "Found another video stream: " << index << ". Ignoring";
                    continue;
                }
                if(hw_decoder_type) {
                    const auto type
                        = find_hw_device_by_name(hw_decoder_type.value().data());
                    const auto hw_pix_fmt = find_hw_pix_fmt(local_decoder, type);
                    decoder_context.set_default_get_format(hw_pix_fmt);
                    decoder_context.hw_decoder_init(type);
                    BOOST_LOG_TRIVIAL(debug) << "Using hardware decoder: "
                                             << av_hwdevice_get_type_name(type);
                }
                decoder_context.guess_frame_rate(input_format_context, stream);
                BOOST_LOG_TRIVIAL(debug)
                    << "VIDEO codec name = " << local_decoder->name << ", stream "
                    << index;
                const AVRational framerate = decoder_context.framerate();
                BOOST_LOG_TRIVIAL(debug)
                    << "decoder: guess_frame_rate = " << framerate << " = "
                    << av_q2d(framerate)
                    << " time_base = " << decoder_context.time_base();
                has_video_stream = true;
                break;
            }
            case AVMEDIA_TYPE_AUDIO: {
                BOOST_LOG_TRIVIAL(debug)
                    << "AUDIO codec name = " << local_decoder->name << ", stream "
                    << index;
                if(decoder_context.ch_layout().order == AV_CHANNEL_ORDER_UNSPEC) {
                    av_channel_layout_default(
                        &decoder_context.ch_layout(),
                        decoder_context.ch_layout().nb_channels);
                }
                break;
            }
            default:
                BOOST_LOG_TRIVIAL(fatal)
                    << "Unexpected stream type in input file: "
                    << "index = " << index
                    << ", type = " << av_get_media_type_string(codec_type);
                throw std::runtime_error("Unreachable!");
        }
        // Set the packet timebase for the decoder.
        decoder_context.set_pkt_timebase(stream->time_base);
        decoder_context.open();
        BOOST_LOG_TRIVIAL(debug) << "stream " << index << ":"
                                 << " timebase = " << stream->time_base
                                 << " r_frame_rate = " << stream->r_frame_rate
                                 << " start_time = " << stream->start_time;

        ret.emplace(index, std::move(decoder_context));
    }
    return ret;
}

int find_video_stream_index(const detail::ScopedAvFormatInput& input_format_context)
{
    int ret = -1;
    for(auto&& [index, stream] :
        boost::adaptors::index(input_format_context.streams())) {
        const auto codec_type = stream->codecpar->codec_type;
        if(codec_type == AVMEDIA_TYPE_VIDEO) {
            ret = static_cast<int>(index);
            break;
        }
    }
    return ret;
}
} // namespace

InputDevice open_input_device(
    const char* const url,
    const std::optional<std::string>& file_format,
    ScopedAvDictionary& demuxer_options,
    const std::optional<std::string>& hw_decoder_type)
{
    BOOST_LOG_FUNCTION();
    unique_register_all_ffmpeg_devices();

    auto input_format_context
        = create_input_format_context(url, file_format, demuxer_options);

    auto decoder_contexts
        = create_decoder_contexts(input_format_context, hw_decoder_type);
    const int video_stream_index = find_video_stream_index(input_format_context);
    if(video_stream_index == -1) {
        BOOST_LOG_TRIVIAL(fatal) << "Input file does not contain video streams!";
        std::exit(1);
    }
    return InputDevice(std::make_unique<InputDevice::Impl>(
        std::move(input_format_context),
        std::move(decoder_contexts)));
}
} // namespace vehlwn::ffmpeg
