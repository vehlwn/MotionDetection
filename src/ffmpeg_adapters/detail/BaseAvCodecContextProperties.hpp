#pragma once

#include <libavutil/pixfmt.h>
#include <stdexcept>
extern "C" {
#include <libavcodec/avcodec.h>
}

#include "ScopedAvFormatInput.hpp"
#include "ScopedHwDeviceCtx.hpp"

namespace vehlwn::ffmpeg::detail {
class BaseAvCodecContextProperties {
public:
    BaseAvCodecContextProperties() = default;
    BaseAvCodecContextProperties(const BaseAvCodecContextProperties&) = default;
    BaseAvCodecContextProperties(BaseAvCodecContextProperties&&) = default;
    BaseAvCodecContextProperties& operator=(const BaseAvCodecContextProperties&)
        = default;
    BaseAvCodecContextProperties& operator=(BaseAvCodecContextProperties&&)
        = default;
    virtual ~BaseAvCodecContextProperties() = default;

    [[nodiscard]] virtual AVCodecContext* raw() const = 0;

    void guess_frame_rate(ScopedAvFormatInput& ctx, AVStream* stream) const
    {
        raw()->framerate = ctx.guess_frame_rate(stream);
    }
    [[nodiscard]] AVRational framerate() const
    {
        return raw()->framerate;
    }
    void set_width(const int x) const
    {
        raw()->width = x;
    }
    [[nodiscard]] int width() const
    {
        return raw()->width;
    }
    void set_height(const int x) const
    {
        raw()->height = x;
    }
    [[nodiscard]] int height() const
    {
        return raw()->height;
    }
    void set_pix_fmt(const AVPixelFormat x) const
    {
        raw()->pix_fmt = x;
    }
    [[nodiscard]] AVPixelFormat pix_fmt() const
    {
        return raw()->pix_fmt;
    }
    [[nodiscard]] AVMediaType codec_type() const
    {
        return raw()->codec_type;
    }
    void set_time_base(const AVRational x) const
    {
        raw()->time_base = x;
    }
    [[nodiscard]] AVRational time_base() const
    {
        return raw()->time_base;
    }
    [[nodiscard]] AVCodecID codec_id() const
    {
        return raw()->codec_id;
    }
    void set_sample_aspect_ratio(const AVRational x) const
    {
        raw()->sample_aspect_ratio = x;
    }
    [[nodiscard]] AVRational sample_aspect_ratio() const
    {
        return raw()->sample_aspect_ratio;
    }
    void set_sample_rate(const int x) const
    {
        raw()->sample_rate = x;
    }
    [[nodiscard]] int sample_rate() const
    {
        return raw()->sample_rate;
    }
    void set_ch_layout(const AVChannelLayout& x) const
    {
        const int errnum = av_channel_layout_copy(&raw()->ch_layout, &x);
        if(errnum != 0) {
            throw ErrorWithContext("av_channel_layout_copy: ", AvError(errnum));
        }
    }
    [[nodiscard]] AVChannelLayout& ch_layout() const
    {
        return raw()->ch_layout;
    }
    void set_sample_fmt(AVSampleFormat x) const
    {
        raw()->sample_fmt = x;
    }
    [[nodiscard]] AVSampleFormat sample_fmt() const
    {
        return raw()->sample_fmt;
    }
    void set_pkt_timebase(const AVRational x) const
    {
        raw()->pkt_timebase = x;
    }
    void set_flags(const int x) const
    {
        raw()->flags = x;
    }
    [[nodiscard]] unsigned flags() const
    {
        return static_cast<unsigned>(raw()->flags);
    }
    [[nodiscard]] unsigned codec_capabilities() const
    {
        return static_cast<unsigned>(raw()->codec->capabilities);
    }
    [[nodiscard]] int frame_size() const
    {
        return raw()->frame_size;
    }
    void set_max_b_frames(const int x) const
    {
        raw()->max_b_frames = x;
    }
    void set_gop_size(const int x) const
    {
        raw()->gop_size = x;
    }
    void set_default_get_format(const AVPixelFormat hw_pix_fmt)
    {
        set_hw_pix_fmt(hw_pix_fmt);
        raw()->get_format = avcodec_default_get_format;
    }
    virtual void set_hw_pix_fmt(AVPixelFormat hw_pix_fmt) = 0;
    void hw_decoder_init(const AVHWDeviceType type)
    {
        auto& hw_device_ctx = raw()->hw_device_ctx;
        const int err
            = av_hwdevice_ctx_create(&hw_device_ctx, type, nullptr, nullptr, 0);
        if(err < 0) {
            throw std::runtime_error("Failed to create specified HW device");
        }
        init_hw_device(ScopedHwDeviceCtx(av_buffer_ref(hw_device_ctx)));
    }
    virtual void init_hw_device(ScopedHwDeviceCtx&&) = 0;
};
} // namespace vehlwn::ffmpeg::detail
