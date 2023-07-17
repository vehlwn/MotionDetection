#pragma once

#include <cstddef>
#include <stdexcept>

extern "C" {
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

#include <opencv2/core/mat.hpp>

#include "../ErrorWithContext.hpp"
#include "AvError.hpp"
#include "AvFrameAdapters.hpp"

namespace vehlwn::ffmpeg::detail {
class SwsPixelConverter {
    SwsContext* m_raw = nullptr;
    AVPixelFormat m_dst_format = AV_PIX_FMT_NONE;

    auto as_tuple() noexcept
    {
        return std::tie(m_raw, m_dst_format);
    }

public:
    SwsPixelConverter(
        const int w,
        const int h,
        const AVPixelFormat srcFormat,
        const AVPixelFormat dstFormat)
        : m_raw(sws_getContext(
            w,
            h,
            srcFormat,
            w,
            h,
            dstFormat,
            0,
            nullptr,
            nullptr,
            nullptr))
    {
        if(m_raw == nullptr) {
            throw std::runtime_error("Failed to create SwsContext");
        }
        m_dst_format = dstFormat;
    }
    SwsPixelConverter(const SwsPixelConverter&) = delete;
    SwsPixelConverter(SwsPixelConverter&& rhs) noexcept
    {
        swap(rhs);
    }
    ~SwsPixelConverter()
    {
        sws_freeContext(m_raw);
    }
    SwsPixelConverter& operator=(const SwsPixelConverter&) = delete;
    SwsPixelConverter& operator=(SwsPixelConverter&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }
    void swap(SwsPixelConverter& rhs) noexcept
    {
        auto tmp = rhs.as_tuple();
        as_tuple().swap(tmp);
    }

    [[nodiscard]] OwningAvframe scale_video(const OwningAvframe& frame) const
    {
        if(frame.height() == 0 || frame.width() == 0) {
            throw std::runtime_error("scale_video accepts only VIDEO frames!");
        }
        OwningAvframe ret = VideoAvFrameBuilder()
                                .format(m_dst_format)
                                .width(frame.width())
                                .height(frame.height())
                                .get_buffer();
        scale_impl(
            frame.data(),
            frame.linesize(),
            0,
            frame.height(),
            ret.data(),
            ret.linesize());
        return ret;
    }

    [[nodiscard]] cv::Mat frame_to_cv_mat(const OwningAvframe& frame) const
    {
        if(frame.height() == 0 || frame.width() == 0) {
            throw std::runtime_error("frame_to_cv_mat accepts only VIDEO frames!");
        }
        auto tmp = scale_video(frame);
        int type = 0;
        switch(m_dst_format) {
            case AV_PIX_FMT_BGR24:
                type = CV_8UC3;
                break;
            default:
                throw std::runtime_error("m_dst_format not implemented");
        }
        auto ret = cv::Mat(
                       frame.height(),
                       frame.width(),
                       type,
                       tmp.data()[0],
                       static_cast<std::size_t>(tmp.linesize()[0]))
                       .clone();
        return ret;
    }

private:
    void scale_impl(
        const std::uint8_t* const* const srcSlice,
        const int* const srcStride,
        const int srcSliceY,
        const int srcSliceH,
        uint8_t* const* const dst,
        const int* const dstStride) const
    {
        const int errnum = sws_scale(
            m_raw,
            srcSlice,
            srcStride,
            srcSliceY,
            srcSliceH,
            dst,
            dstStride);
        if(errnum < 0) {
            throw ErrorWithContext("sws_scale failed: ", AvError(errnum));
        }
    }
};
} // namespace vehlwn::ffmpeg::detail
