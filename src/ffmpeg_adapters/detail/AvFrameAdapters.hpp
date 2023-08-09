#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <utility>

#include <opencv2/core/mat.hpp>

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
}

#include "../ErrorWithContext.hpp"
#include "AvError.hpp"

namespace vehlwn::ffmpeg::detail {
class OwningAvframe {
    AVFrame* m_raw = nullptr;

public:
    OwningAvframe()
        : m_raw(av_frame_alloc())
    {
        if(m_raw == nullptr) {
            throw std::runtime_error("failed to allocate memory for AVFrame");
        }
    }
    OwningAvframe(const OwningAvframe&) = delete;
    OwningAvframe(OwningAvframe&& rhs) noexcept
    {
        swap(rhs);
    }
    ~OwningAvframe()
    {
        av_frame_free(&m_raw);
    }
    OwningAvframe& operator=(const OwningAvframe&) = delete;
    OwningAvframe& operator=(OwningAvframe&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }
    void swap(OwningAvframe& rhs) noexcept
    {
        std::swap(m_raw, rhs.m_raw);
    }
    AVFrame* raw()
    {
        return m_raw;
    }
    [[nodiscard]] AVFrame const* raw() const
    {
        return m_raw;
    }
    [[nodiscard]] AVPictureType pict_type() const
    {
        return m_raw->pict_type;
    }
    void set_pict_type(const AVPictureType x) const
    {
        m_raw->pict_type = x;
    }
    void set_pts(const std::int64_t x) const
    {
        m_raw->pts = x;
    }
    [[nodiscard]] std::int64_t pts() const
    {
        return m_raw->pts;
    }
    [[nodiscard]] std::int64_t pkt_dts() const
    {
        return m_raw->pkt_dts;
    }
    [[nodiscard]] std::int64_t best_effort_timestamp() const
    {
        return m_raw->best_effort_timestamp;
    }
    [[nodiscard]] const std::uint8_t** extended_data() const
    {
        // NOLINTNEXTLINE: adding const to non const data
        return const_cast<const std::uint8_t**>(m_raw->extended_data);
    }
    [[nodiscard]] int nb_samples() const
    {
        return m_raw->nb_samples;
    }
    void set_nb_samples(const int x) const
    {
        m_raw->nb_samples = x;
    }
    void set_ch_layout(const AVChannelLayout& x) const
    {
        const int errnum = av_channel_layout_copy(&m_raw->ch_layout, &x);
        if(errnum != 0) {
            throw ErrorWithContext("av_channel_layout_copy: ", AvError(errnum));
        }
    }
    void set_format(const int x) const
    {
        m_raw->format = x;
    }
    [[nodiscard]] AVPixelFormat format() const
    {
        return static_cast<AVPixelFormat>(m_raw->format);
    }
    void set_sample_rate(const int x) const
    {
        m_raw->sample_rate = x;
    }
    [[nodiscard]] const std::uint8_t* const* data() const
    {
        return static_cast<const std::uint8_t* const*>(m_raw->data);
    }
    std::uint8_t** data()
    {
        return static_cast<std::uint8_t**>(m_raw->data);
    }
    [[nodiscard]] const int* linesize() const
    {
        return static_cast<const int*>(m_raw->linesize);
    }
    [[nodiscard]] int height() const
    {
        return m_raw->height;
    }
    void set_height(const int x) const
    {
        m_raw->height = x;
    }
    [[nodiscard]] int width() const
    {
        return m_raw->width;
    }
    void set_width(const int x) const
    {
        m_raw->width = x;
    }
    void copy_props_from(const OwningAvframe& other)
    {
        const int errnum = av_frame_copy_props(raw(), other.raw());
        if(errnum < 0) {
            throw ErrorWithContext("av_frame_copy_props failed", AvError(errnum));
        }
    }
    void transfer_hwdata_from(const OwningAvframe& other)
    {
        const int errnum = av_hwframe_transfer_data(raw(), other.raw(), 0);
        if(errnum < 0) {
            throw ErrorWithContext(
                "Error transferring the data to system memory: ",
                AvError(errnum));
        }
    }
    [[nodiscard]] cv::Mat copy_to_cv_mat() const
    {
        if(format() != AV_PIX_FMT_BGR24) {
            throw std::runtime_error(
                std::string(
                    "Failed to copy AVFrame to cv::Mat: expected BGR24 format, got ")
                + av_get_pix_fmt_name(format()));
        }
        auto ret = cv::Mat(height(), width(), CV_8UC3);
        const auto ffmpeg_step = static_cast<ptrdiff_t>(linesize()[0]);
        const auto opencv_step = static_cast<ptrdiff_t>(width()) * 3;
        for(int i = 0; i < height(); i++) {
            const auto begin = data()[0] + ffmpeg_step * i;
            const auto end = begin + linesize()[0];
            std::copy(begin, end, ret.data + opencv_step * i);
        }
        return ret;
    }
};

class VideoAvFrameBuilder {
    std::optional<AVPixelFormat> m_format;
    std::optional<int> m_width;
    std::optional<int> m_height;

public:
    VideoAvFrameBuilder& format(const AVPixelFormat x)
    {
        m_format = x;
        return *this;
    }
    VideoAvFrameBuilder& width(const int x)
    {
        m_width = x;
        return *this;
    }
    VideoAvFrameBuilder& height(const int x)
    {
        m_height = x;
        return *this;
    }

    [[nodiscard]] OwningAvframe get_buffer() const
    {
        OwningAvframe ret;
        if(m_format) {
            ret.set_format(m_format.value());
        } else {
            throw std::runtime_error(
                "VideoAvFrameBuilder::get_buffer: format is not set");
        }
        if(m_width) {
            ret.set_width(m_width.value());
        } else {
            throw std::runtime_error(
                "VideoAvFrameBuilder::get_buffer: width is not set");
        }
        if(m_height) {
            ret.set_height(m_height.value());
        } else {
            throw std::runtime_error(
                "VideoAvFrameBuilder::get_buffer: height is not set");
        }
        const int errnum = av_frame_get_buffer(ret.raw(), 0);
        if(errnum < 0) {
            throw ErrorWithContext("av_frame_get_buffer failed: ", AvError(errnum));
        }
        return ret;
    }
};

class AudioAvFrameBuilder {
    std::optional<AVSampleFormat> m_format;
    std::optional<int> m_nb_samples;
    std::optional<AVChannelLayout> m_ch_layout;

public:
    AudioAvFrameBuilder& format(const AVSampleFormat x)
    {
        m_format = x;
        return *this;
    }
    AudioAvFrameBuilder& nb_samples(const int x)
    {
        m_nb_samples = x;
        return *this;
    }
    AudioAvFrameBuilder& ch_layout(const AVChannelLayout& x)
    {
        m_ch_layout = AVChannelLayout{};
        const int errnum = av_channel_layout_copy(&m_ch_layout.value(), &x);
        if(errnum != 0) {
            throw ErrorWithContext("av_channel_layout_copy: ", AvError(errnum));
        }
        return *this;
    }
    [[nodiscard]] OwningAvframe get_buffer() const
    {
        OwningAvframe ret;
        if(m_format) {
            ret.set_format(m_format.value());
        } else {
            throw std::runtime_error(
                "AudioAvFrameBuilder::get_buffer: format is not set");
        }
        if(m_nb_samples) {
            ret.set_nb_samples(m_nb_samples.value());
        } else {
            throw std::runtime_error(
                "VideoAvFrameBuilder::get_buffer: nb_samples is not set");
        }
        if(m_ch_layout) {
            ret.set_ch_layout(m_ch_layout.value());
        } else {
            throw std::runtime_error(
                "VideoAvFrameBuilder::get_buffer: ch_layout is not set");
        }
        const int errnum = av_frame_get_buffer(ret.raw(), 0);
        if(errnum < 0) {
            throw ErrorWithContext("av_frame_get_buffer failed: ", AvError(errnum));
        }
        return ret;
    }
};
} // namespace vehlwn::ffmpeg::detail
