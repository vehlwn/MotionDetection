#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "ScopedAvFormatInput.hpp"

namespace vehlwn::ffmpeg ::detail {
class BaseAvCodecContextProperties {
protected:
    AVCodecContext* m_raw = nullptr;
    BaseAvCodecContextProperties() = default;

public:
    void guess_frame_rate(ScopedAvFormatInput& ctx, AVStream* stream) const
    {
        m_raw->framerate = ctx.guess_frame_rate(stream);
    }
    AVRational framerate() const
    {
        return m_raw->framerate;
    }
    void set_width(const int x) const
    {
        m_raw->width = x;
    }
    int width() const
    {
        return m_raw->width;
    }
    void set_height(const int x) const
    {
        m_raw->height = x;
    }
    int height() const
    {
        return m_raw->height;
    }
    void set_pix_fmt(const AVPixelFormat x) const
    {
        m_raw->pix_fmt = x;
    }
    AVPixelFormat pix_fmt() const
    {
        return m_raw->pix_fmt;
    }
    AVMediaType codec_type() const
    {
        return m_raw->codec_type;
    }
    void set_time_base(const AVRational x) const
    {
        m_raw->time_base = x;
    }
    AVRational time_base() const
    {
        return m_raw->time_base;
    }
    AVCodecID codec_id() const
    {
        return m_raw->codec_id;
    }
    void set_sample_aspect_ratio(const AVRational x) const
    {
        m_raw->sample_aspect_ratio = x;
    }
    AVRational sample_aspect_ratio() const
    {
        return m_raw->sample_aspect_ratio;
    }
    void set_sample_rate(const int x) const
    {
        m_raw->sample_rate = x;
    }
    int sample_rate() const
    {
        return m_raw->sample_rate;
    }
    void set_ch_layout(const AVChannelLayout& x) const
    {
        const int errnum = av_channel_layout_copy(&m_raw->ch_layout, &x);
        if(errnum != 0) {
            throw ErrorWithContext("av_channel_layout_copy: ", AvError(errnum));
        }
    }
    AVChannelLayout& ch_layout() const
    {
        return m_raw->ch_layout;
    }
    void set_sample_fmt(AVSampleFormat x) const
    {
        m_raw->sample_fmt = x;
    }
    AVSampleFormat sample_fmt() const
    {
        return m_raw->sample_fmt;
    }
    void set_pkt_timebase(const AVRational x) const
    {
        m_raw->pkt_timebase = x;
    }
    void set_bit_rate(const std::int64_t x) const
    {
        m_raw->bit_rate = x;
    }
    void set_flags(const int x) const
    {
        m_raw->flags = x;
    }
    int flags() const
    {
        return m_raw->flags;
    }
    int codec_capabilities() const
    {
        return m_raw->codec->capabilities;
    }
    int frame_size() const
    {
        return m_raw->frame_size;
    }
    void set_max_b_frames(const int x) const
    {
        m_raw->max_b_frames = x;
    }
    void set_gop_size(const int x) const
    {
        m_raw->gop_size = x;
    }
};
} // namespace vehlwn::ffmpeg::detail
