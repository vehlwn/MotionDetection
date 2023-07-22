#include "OutputFile.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/core/span.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/for_each.hpp>

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/mathematics.h>
#include <libavutil/pixdesc.h>
#include <libavutil/rational.h>
}

#include "AVRationalOutput.hpp"
#include "AvFrameAdapters.hpp"
#include "AvPacketAdapters.hpp"
#include "ScopedAvAudioFifo.hpp"
#include "ScopedAvFormatInput.hpp"
#include "ScopedAvFormatOutput.hpp"
#include "ScopedAvSAmplesBuffer.hpp"
#include "ScopedEncoderContext.hpp"
#include "SwrResampler.hpp"
#include "SwsPixelConverter.hpp"

namespace vehlwn::ffmpeg::detail {
struct OutputFile::Impl {
    ScopedAvFormatOutput out_format_context;
    std::vector<ScopedEncoderContext> encoder_contexts;
    std::map<int, int> in_out_stream_mapping;
    std::map<int, ScopedAvAudioFifo> audio_fifos;
    std::map<int, ScopedSwrResampler> resamplers;
    std::optional<SwsPixelConverter> video_pix_converter;
    std::map<int, AVRational> orig_stream_time_bases;

    std::map<int, std::int64_t> start_times;
    std::map<int, std::int64_t> next_pts;

    Impl(
        ScopedAvFormatOutput&& out_format_context_,
        std::vector<ScopedEncoderContext>&& encoder_contexts_,
        std::map<int, int>&& in_out_stream_mapping_,
        std::map<int, ScopedAvAudioFifo>&& audio_fifos_,
        std::map<int, ScopedSwrResampler>&& resamplers_,
        std::optional<SwsPixelConverter>&& video_pix_converter_,
        std::map<int, AVRational>&& orig_stream_time_bases_)
        : out_format_context(std::move(out_format_context_))
        , encoder_contexts(std::move(encoder_contexts_))
        , in_out_stream_mapping(std::move(in_out_stream_mapping_))
        , audio_fifos(std::move(audio_fifos_))
        , resamplers(std::move(resamplers_))
        , video_pix_converter(std::move(video_pix_converter_))
        , orig_stream_time_bases(std::move(orig_stream_time_bases_))
    {
        BOOST_LOG_FUNCTION();
        if((out_format_context.oformat_flags() & static_cast<unsigned>(AVFMT_NOFILE))
           == 0) {
            out_format_context.avio_open();
        }
        // init muxer, write output file header
        out_format_context.write_header();
        boost::for_each(
            boost::adaptors::index(out_format_context.streams()),
            [&](auto&& elem) {
                BOOST_LOG_TRIVIAL(debug)
                    << "out stream " << elem.index()
                    << ": time_base = " << elem.value()->time_base;
            });
    }

    ~Impl()
    try {
        BOOST_LOG_FUNCTION();
        flush_audio_fifos();
        flush_encoders();
        out_format_context.write_trailer();
    } catch(const std::exception& ex) {
        BOOST_LOG_TRIVIAL(error) << "~Impl dtor: " << ex.what();
    }

    Impl(const Impl&) = delete;
    Impl(Impl&&) = default;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) = default;

    void flush_encoders()
    {
        boost::for_each(boost::adaptors::index(encoder_contexts), [&](auto&& elem) {
            const auto out_stream_index = static_cast<int>(elem.index());
            const auto& encoder_context = elem.value();
            if(!(encoder_context.codec_capabilities()
                 & static_cast<unsigned>(AV_CODEC_CAP_DELAY))) {
                return;
            }
            encode_write_frame_impl(std::nullopt, out_stream_index);
        });
    }

    void flush_audio_fifos()
    {
        boost::for_each(audio_fifos, [&](const auto& p) {
            const int out_stream_index = p.first;
            const auto& fifo = p.second;
            const auto& encoder_context
                = encoder_contexts.at(static_cast<std::size_t>(out_stream_index));
            while(fifo.size() > 0) {
                consume_encode_audio_fifo(fifo, encoder_context, out_stream_index);
            }
        });
    }

    void process_audio_frame(const OwningAvframe& frame, const int out_stream_index)
    {
        BOOST_LOG_FUNCTION();
        const auto& encoder_context
            = encoder_contexts.at(static_cast<std::size_t>(out_stream_index));
        const auto& fifo = audio_fifos.at(out_stream_index);
        ScopedAvSAmplesBuffer converted_buffer(
            encoder_context.ch_layout().nb_channels,
            frame.nb_samples(),
            encoder_context.sample_fmt());
        const auto& resampler = resamplers.at(out_stream_index);
        // Convert the input samples to the desired output sample format
        resampler.convert(
            frame.extended_data(),
            converted_buffer.data(),
            frame.nb_samples());
        // Add the converted input samples to the FIFO buffer for later processing
        fifo.add_samples(converted_buffer.data(), frame.nb_samples());
        // Make sure that there is one frame worth of samples in the FIFO buffer.
        // Since the decoder's and the encoder's frame size may differ, we need the
        // FIFO buffer to store as many frames worth of input samples that they
        // make up at least one frame worth of output samples.
        const int out_frame_size = encoder_context.frame_size();
        BOOST_LOG_TRIVIAL(trace) << "audio stream " << out_stream_index
                                 << ": nb_samples = " << frame.nb_samples()
                                 << ": out_frame_size = " << out_frame_size;
        while(fifo.size() >= out_frame_size) {
            consume_encode_audio_fifo(fifo, encoder_context, out_stream_index);
        }
    }

    void consume_encode_audio_fifo(
        const ScopedAvAudioFifo& fifo,
        const ScopedEncoderContext& encoder_context,
        const int out_stream_index)
    {
        const int out_frame_size = encoder_context.frame_size();
        // Each frame will contain exactly out_frame_size samples except the last
        const int fifo_frame_size = std::min(fifo.size(), out_frame_size);
        // Allocate the samples of the created frame. This call will make sure
        // that the audio frame can hold as many samples as specified.
        // Read as many samples from the FIFO buffer as required to fill the
        OwningAvframe tmp_frame = AudioAvFrameBuilder()
                                      .format(encoder_context.sample_fmt())
                                      .nb_samples(fifo_frame_size)
                                      .ch_layout(encoder_context.ch_layout())
                                      .get_buffer();
        tmp_frame.set_sample_rate(encoder_context.sample_rate());
        // NOLINTNEXTLINE: cast uint8_t** to void**
        fifo.read(reinterpret_cast<void**>(tmp_frame.data()), fifo_frame_size);
        encode_write_frame_impl(std::cref(tmp_frame), out_stream_index);
    }

    void process_video_frame(const OwningAvframe& frame, const int out_stream_index)
    {
        if(video_pix_converter) {
            const auto converted = video_pix_converter.value().scale_video(frame);
            converted.set_pts(frame.pts());
            encode_write_frame_impl(std::cref(converted), out_stream_index);
        } else {
            encode_write_frame_impl(std::cref(frame), out_stream_index);
        }
    }

    void calc_pts(const OwningAvframe& frame, const int out_stream_index)
    {
        BOOST_LOG_FUNCTION();
        const auto& encoder_context
            = encoder_contexts.at(static_cast<std::size_t>(out_stream_index));
        const auto frame_type = encoder_context.codec_type();
        const auto in_stream_tb = orig_stream_time_bases.at(out_stream_index);
        const auto out_stream_tb
            = out_format_context
                  .streams()[static_cast<std::size_t>(out_stream_index)]
                  ->time_base;
        switch(frame_type) {
            case AVMEDIA_TYPE_VIDEO:
                if(frame.pts() == AV_NOPTS_VALUE) {
                    BOOST_LOG_TRIVIAL(error)
                        << "Video frame does not have pts value";
                } else {
                    auto start_time_it = start_times.find(out_stream_index);
                    if(start_time_it == start_times.end()) {
                        BOOST_LOG_TRIVIAL(debug)
                            << "Setting out video stream start_time = "
                            << frame.pts();
                        start_time_it
                            = start_times.emplace(out_stream_index, frame.pts())
                                  .first;
                    }
                    const auto new_pts = frame.pts() - start_time_it->second;
                    const auto rescaled_pts
                        = av_rescale_q(new_pts, in_stream_tb, out_stream_tb);
                    frame.set_pts(rescaled_pts);
                }
                break;
            case AVMEDIA_TYPE_AUDIO:
                if(frame.pts() == AV_NOPTS_VALUE) {
                    auto new_pts_it = next_pts.find(out_stream_index);
                    if(new_pts_it == next_pts.end()) {
                        new_pts_it = next_pts.emplace(out_stream_index, 0).first;
                    }
                    auto& new_pts = new_pts_it->second;
                    const auto enc_tb = encoder_context.time_base();
                    const auto rescaled_pts
                        = av_rescale_q(new_pts, enc_tb, out_stream_tb);
                    frame.set_pts(rescaled_pts);
                    new_pts += frame.nb_samples();
                } else {
                    BOOST_LOG_TRIVIAL(error)
                        << "Unexpected! Audio resampler returned valid pts!";
                }
                break;
            default:
                throw std::runtime_error("Unreachable!");
        }
    }

    void encode_write_frame_impl(
        const std::optional<std::reference_wrapper<const OwningAvframe>> frame,
        const int out_stream_index)
    {
        auto& encoder_context
            = encoder_contexts.at(static_cast<std::size_t>(out_stream_index));
        if(frame) {
            const auto& unpacked_frame = frame.value().get();
            unpacked_frame.set_pict_type(AV_PICTURE_TYPE_NONE);
            calc_pts(unpacked_frame, out_stream_index);
            encoder_context.send_frame(unpacked_frame);
        } else {
            encoder_context.send_flush_frame();
        }
        while(true) {
            auto encoded_result = encoder_context.receive_packet();
            if(auto* const enc_packet
               = std::get_if<OwningAvPacket>(&encoded_result)) {
                // prepare packet for muxing
                enc_packet->set_stream_index(out_stream_index);
                // mux encoded frame
                out_format_context.interleaved_write_packet(*enc_packet);
            } else if(std::holds_alternative<ScopedEncoderContext::Again>(
                          encoded_result)) {
                break;
            }
        }
    }
};

OutputFile::OutputFile(std::unique_ptr<Impl>&& impl)
    : pimpl(std::move(impl))
{}

OutputFile::OutputFile(OutputFile&& rhs) noexcept = default;

OutputFile::~OutputFile() = default;

void OutputFile::encode_write_frame(
    const OwningAvframe& frame,
    const int in_stream_index)
{
    BOOST_LOG_FUNCTION();
    const int out_stream_index = pimpl->in_out_stream_mapping.at(in_stream_index);
    const auto& encoder_context
        = pimpl->encoder_contexts.at(static_cast<std::size_t>(out_stream_index));
    if(encoder_context.codec_type() == AVMEDIA_TYPE_AUDIO) {
        pimpl->process_audio_frame(frame, out_stream_index);
    } else if(encoder_context.codec_type() == AVMEDIA_TYPE_VIDEO) {
        pimpl->process_video_frame(frame, out_stream_index);
    } else {
        throw std::runtime_error("Unreachable!");
    }
}

OutputFile open_output_file(
    const char* const url,
    const std::map<int, ScopedDecoderContext>& decoder_contexts,
    const ScopedAvFormatInput::StreamsView in_streams,
    const std::optional<std::string>& video_bitrate,
    const std::optional<std::string>& audio_bitrate)
{
    BOOST_LOG_FUNCTION();
    auto out_format_context = ScopedAvFormatOutput(url);
    std::vector<ScopedEncoderContext> encoder_contexts;
    std::map<int, int> in_out_stream_mapping;
    std::map<int, ScopedAvAudioFifo> audio_fifos;
    std::map<int, ScopedSwrResampler> resamplers;
    std::optional<SwsPixelConverter> video_pix_converter;

    constexpr auto out_vcodec = AV_CODEC_ID_H264;
    constexpr auto out_acodec = AV_CODEC_ID_AAC;

    int out_stream_counter = 0;
    for(auto&& [in_stream_index, decoder_context] : decoder_contexts) {
        const AVMediaType input_codec_type = decoder_context.codec_type();
        const AVCodec* encoder = nullptr;
        if(input_codec_type == AVMEDIA_TYPE_VIDEO) {
            encoder = avcodec_find_encoder(out_vcodec);
        } else {
            encoder = avcodec_find_encoder(out_acodec);
        }

        auto encoder_context = ScopedEncoderContext(encoder);
        auto encoder_options = ScopedAvDictionary();

        switch(input_codec_type) {
            case AVMEDIA_TYPE_VIDEO: {
                // transcode to same properties
                encoder_context.set_height(decoder_context.height());
                encoder_context.set_width(decoder_context.width());
                encoder_context.set_sample_aspect_ratio(
                    decoder_context.sample_aspect_ratio());
                const auto enc_pix_fmts = [&] {
                    boost::span<const AVPixelFormat> ret;
                    if(!encoder->pix_fmts) {
                        return ret;
                    }
                    const auto* const start = encoder->pix_fmts;
                    const auto* end = start;
                    while(static_cast<int>(*end) != -1) {
                        end++;
                    }
                    ret = {start, end};
                    return ret;
                }();
                if(const auto it
                   = boost::find(enc_pix_fmts, decoder_context.pix_fmt());
                   it != enc_pix_fmts.end()) {
                    encoder_context.set_pix_fmt(*it);
                } else {
                    if(video_pix_converter) {
                        BOOST_LOG_TRIVIAL(warning)
                            << "Found more than one video stream! "
                               "Ignoring others";
                    } else {
                        const auto dst_format = enc_pix_fmts[0];
                        video_pix_converter = SwsPixelConverter(
                            decoder_context.width(),
                            decoder_context.height(),
                            decoder_context.pix_fmt(),
                            dst_format);
                        encoder_context.set_pix_fmt(dst_format);
                    }
                }
                // video time_base can be set to whatever is handy and supported
                // by encoder
                encoder_context.set_time_base(av_inv_q(decoder_context.framerate()));
                // https://support.google.com/youtube/answer/1722171?hl=en
                encoder_context.set_max_b_frames(2);
                encoder_context.set_gop_size(
                    static_cast<int>(av_q2d(decoder_context.framerate()) / 2.0));
                encoder_options.set_str("flags", "+cgop");
                // h264 private options. See ffmpeg -h encoder=h264 and x264 -h.
                encoder_options.set_str("preset", "ultrafast");
                encoder_options.set_str("tune", "zerolatency");
                if(video_bitrate) {
                    encoder_options.set_str("b", video_bitrate.value().data());
                }
                break;
            }
            case AVMEDIA_TYPE_AUDIO: {
                encoder_context.set_sample_rate(decoder_context.sample_rate());
                encoder_context.set_ch_layout(decoder_context.ch_layout());
                // take first format from list of supported formats
                encoder_context.set_sample_fmt(encoder->sample_fmts[0]);
                encoder_context.set_time_base(
                    av_make_q(1, encoder_context.sample_rate()));
                if(audio_bitrate) {
                    encoder_options.set_str("b", audio_bitrate.value().data());
                }

                auto resampler = SwrResamplerBuiler()
                                     .in_ch_layout(&decoder_context.ch_layout())
                                     .in_sample_fmt(decoder_context.sample_fmt())
                                     .in_sample_rate(decoder_context.sample_rate())
                                     .out_ch_layout(&encoder_context.ch_layout())
                                     .out_sample_fmt(encoder_context.sample_fmt())
                                     .out_sample_rate(encoder_context.sample_rate())
                                     .build();
                resamplers.emplace(out_stream_counter, std::move(resampler));
                audio_fifos.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(out_stream_counter),
                    std::forward_as_tuple(
                        encoder_context.sample_fmt(),
                        encoder_context.ch_layout().nb_channels));
                break;
            }
            default:
                BOOST_LOG_TRIVIAL(warning)
                    << "Ignoring input stream " << in_stream_index << " of type "
                    << av_get_media_type_string(input_codec_type);
                continue;
        }

        if(out_format_context.oformat_flags()
           & static_cast<unsigned>(AVFMT_GLOBALHEADER)) {
            encoder_context.set_flags(static_cast<int>(
                encoder_context.flags()
                | static_cast<unsigned>(AV_CODEC_FLAG_GLOBAL_HEADER)));
        }

        AVStream* const out_stream = out_format_context.new_stream();
        out_stream->time_base = encoder_context.time_base();

        BOOST_LOG_TRIVIAL(debug) << "out stream " << out_stream_counter
                                 << ": encoder options = " << encoder_options;
        encoder_context.open(out_stream->codecpar, encoder_options);
        if(encoder_options.size() != 0) {
            BOOST_LOG_TRIVIAL(fatal) << "out stream " << out_stream_counter
                                     << ": options not found = " << encoder_options;
            throw std::runtime_error("Encoder option not found");
        }
        encoder_contexts.emplace_back(std::move(encoder_context));
        in_out_stream_mapping.emplace(in_stream_index, out_stream_counter);
        out_stream_counter++;
    }
    out_format_context.dump_format();

    std::map<int, AVRational> orig_stream_time_bases;
    for(const auto& [in_stream_index, stream] : boost::adaptors::index(in_streams)) {
        if(const auto it
           = in_out_stream_mapping.find(static_cast<int>(in_stream_index));
           it != in_out_stream_mapping.end()) {
            orig_stream_time_bases.emplace(it->second, stream->time_base);
        } else {
            continue;
        }
    }

    return OutputFile(std::make_unique<OutputFile::Impl>(
        std::move(out_format_context),
        std::move(encoder_contexts),
        std::move(in_out_stream_mapping),
        std::move(audio_fifos),
        std::move(resamplers),
        std::move(video_pix_converter),
        std::move(orig_stream_time_bases)));
}
} // namespace vehlwn::ffmpeg::detail
