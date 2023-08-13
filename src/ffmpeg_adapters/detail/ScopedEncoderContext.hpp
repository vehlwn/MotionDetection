#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
}

#include <stdexcept>
#include <variant>

#include "../ErrorWithContext.hpp"
#include "AvError.hpp"
#include "AvFrameAdapters.hpp"
#include "AvPacketAdapters.hpp"
#include "BaseAvCodecContextProperties.hpp"
#include "HardwareHelpers.hpp"

namespace vehlwn::ffmpeg::detail {
class ScopedEncoderContext : public BaseAvCodecContextProperties {
    AVCodecContext* m_raw = nullptr;
    const AVCodec* m_codec = nullptr;
    AVPixelFormat m_hw_pix_fmt = AV_PIX_FMT_NONE;

    [[nodiscard]] auto as_tuple()
    {
        return std::tie(m_raw, m_codec, m_hw_pix_fmt);
    }

public:
    explicit ScopedEncoderContext(const AVCodec* const codec)
        : m_raw(avcodec_alloc_context3(codec))
        , m_codec(codec)
    {
        if(m_raw == nullptr) {
            throw std::runtime_error(
                "Failed to allocated memory for AVCodecContext");
        }
    }
    ScopedEncoderContext(const ScopedEncoderContext&) = delete;
    ScopedEncoderContext(ScopedEncoderContext&& rhs) noexcept
    {
        swap(rhs);
    }
    ~ScopedEncoderContext()
    {
        avcodec_free_context(&m_raw);
    }
    ScopedEncoderContext& operator=(const ScopedEncoderContext&) = delete;
    ScopedEncoderContext& operator=(ScopedEncoderContext&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }

    [[nodiscard]] AVCodecContext* raw() const override
    {
        return m_raw;
    }

    void open(AVCodecParameters* const par, ScopedAvDictionary& opts) const
    {
        int errnum = avcodec_open2(m_raw, m_codec, opts.double_ptr());
        if(errnum < 0) {
            throw ErrorWithContext("Cannot open encoder: ", AvError(errnum));
        }
        errnum = avcodec_parameters_from_context(par, m_raw);
        if(errnum < 0) {
            throw ErrorWithContext(
                "Failed to copy encoder parameters to output stream: ",
                AvError(errnum));
        }
    }

    void swap(ScopedEncoderContext& rhs) noexcept
    {
        auto other = rhs.as_tuple();
        as_tuple().swap(other);
    }

    void send_frame(const OwningAvframe& frame) const
    {
        if(raw()->hw_device_ctx != nullptr) {
            auto hw_frame = OwningAvframe();
            hw_frame.get_hw_buffer(raw()->hw_frames_ctx);
            hw_frame.copy_props_from(frame);
            hw_frame.transfer_hwdata_from(frame);
            send_frame_impl(hw_frame.raw());
        } else {
            send_frame_impl(frame.raw());
        }
    }
    void send_flush_frame() const
    {
        send_frame_impl(nullptr);
    }

    struct Again {};
    using ReceivePacketResult = std::variant<OwningAvPacket, Again>;

    ReceivePacketResult receive_packet()
    {
        OwningAvPacket ret;
        const int errnum = avcodec_receive_packet(m_raw, ret.raw());
        if(errnum < 0) {
            if(errnum == AVERROR(EAGAIN) || errnum == AVERROR_EOF) {
                return Again{};
            }
            throw ErrorWithContext(
                "avcodec_receive_packet failed: ",
                AvError(errnum));
        }
        return ret;
    }
    void set_hw_pix_fmt(const AVPixelFormat hw_pix_fmt) override
    {
        m_hw_pix_fmt = hw_pix_fmt;
    }

    struct HwFramesContextParams {
        int width = 0;
        int height = 0;
    };
    void create_hw_frames(const HwFramesContextParams& params)
    {
        if(m_hw_pix_fmt == AV_PIX_FMT_NONE) {
            throw std::runtime_error(
                "You must set hw_pix_fmt before calling create_hw_frames");
        }
        if(raw()->hw_device_ctx == nullptr) {
            throw std::runtime_error(
                "You must call init_hw_device before calling create_hw_frames");
        }
        auto hw_frames_ref = av_hwframe_ctx_alloc(raw()->hw_device_ctx);
        if(hw_frames_ref == nullptr) {
            throw std::runtime_error("av_hwframe_ctx_alloc failed");
        }

        // NOLINTBEGIN: this is the recommended way of getting frames context from
        // vaapi_encode.c example
        const auto frames_ctx
            = reinterpret_cast<AVHWFramesContext*>(hw_frames_ref->data);
        // NOLINTEND
        frames_ctx->format = m_hw_pix_fmt;
        frames_ctx->sw_format = hw_helpers::DEFAULT_SW_FORMAT;
        frames_ctx->width = params.width;
        frames_ctx->height = params.height;
        frames_ctx->initial_pool_size = hw_helpers::DEFAULT_HW_POOL_SIZE;

        const int errnum = av_hwframe_ctx_init(hw_frames_ref);
        if(errnum < 0) {
            throw ErrorWithContext("av_hwframe_ctx_init failed", AvError(errnum));
        }
        raw()->hw_frames_ctx = hw_frames_ref;
    }

private:
    void send_frame_impl(AVFrame const* frame) const
    {
        const int errnum = avcodec_send_frame(m_raw, frame);
        if(errnum < 0) {
            throw ErrorWithContext("avcodec_send_frame failed: ", AvError(errnum));
        }
    }
};
} // namespace vehlwn::ffmpeg::detail
