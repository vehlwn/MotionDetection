#pragma once

#include <libavutil/pixfmt.h>
#include <stdexcept>
#include <variant>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
}

#include "../ErrorWithContext.hpp"
#include "AvError.hpp"
#include "AvFrameAdapters.hpp"
#include "AvPacketAdapters.hpp"
#include "BaseAvCodecContextProperties.hpp"

namespace vehlwn::ffmpeg::detail {
class ScopedDecoderContext : public BaseAvCodecContextProperties {
    AVCodecContext* m_raw = nullptr;
    const AVCodec* m_codec = nullptr;
    AVPixelFormat m_hw_pix_fmt = AV_PIX_FMT_NONE;

    [[nodiscard]] auto as_tuple()
    {
        return std::tie(m_raw, m_codec, m_hw_pix_fmt);
    }

public:
    ScopedDecoderContext(
        const AVCodec* const codec,
        const AVCodecParameters* const par)
        : m_raw(avcodec_alloc_context3(codec))
        , m_codec(codec)
    {
        if(m_raw == nullptr) {
            throw std::runtime_error(
                "Failed to allocated memory for AVCodecContext");
        }
        int errnum = avcodec_parameters_to_context(m_raw, par);
        if(errnum < 0) {
            throw ErrorWithContext(
                "avcodec_parameters_to_context failed: ",
                AvError(errnum));
        }
    }
    ScopedDecoderContext(const ScopedDecoderContext&) = delete;
    ScopedDecoderContext(ScopedDecoderContext&& rhs) noexcept
    {
        swap(rhs);
    }
    ~ScopedDecoderContext()
    {
        avcodec_free_context(&m_raw);
    }
    ScopedDecoderContext& operator=(const ScopedDecoderContext&) = delete;
    ScopedDecoderContext& operator=(ScopedDecoderContext&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }
    void open()
    {
        const int errnum = avcodec_open2(m_raw, m_codec, nullptr);
        if(errnum < 0) {
            throw ErrorWithContext("avcodec_open2 failed: ", AvError(errnum));
        }
    }
    void swap(ScopedDecoderContext& rhs)
    {
        auto other = rhs.as_tuple();
        as_tuple().swap(other);
    }

    [[nodiscard]] AVCodecContext* raw() const override
    {
        return m_raw;
    }

    void send_packet(const OwningAvPacket& packet) const
    {
        const int errnum = avcodec_send_packet(m_raw, packet.raw());
        if(errnum < 0) {
            throw ErrorWithContext("avcodec_send_packet failed: ", AvError(errnum));
        }
    }

    struct Again {};
    using ReceiveFrameResult = std::variant<OwningAvframe, Again>;
    [[nodiscard]] ReceiveFrameResult receive_frame() const
    {
        OwningAvframe frame;
        const int errnum = avcodec_receive_frame(m_raw, frame.raw());
        if(errnum < 0) {
            if(errnum == AVERROR(EAGAIN)) {
                return Again{};
            }
            throw ErrorWithContext(
                "avcodec_receive_frame failed: ",
                AvError(errnum));
        }
        OwningAvframe tmp_frame;
        if(frame.format() == m_hw_pix_fmt) {
            // retrieve data from GPU to CPU
            OwningAvframe sw_frame;
            sw_frame.transfer_hwdata_from(frame);
            sw_frame.copy_props_from(frame);
            tmp_frame = std::move(sw_frame);
        } else {
            tmp_frame = std::move(frame);
        }
        return tmp_frame;
    }
    void set_default_get_format() const
    {
        raw()->get_format = avcodec_default_get_format;
    }
    void set_hw_pix_fmt(const AVPixelFormat hw_pix_fmt) override
    {
        m_hw_pix_fmt = hw_pix_fmt;
    }
};
} // namespace vehlwn::ffmpeg::detail
