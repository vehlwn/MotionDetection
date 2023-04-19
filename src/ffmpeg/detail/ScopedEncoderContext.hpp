#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <stdexcept>
#include <variant>

#include "AvError.hpp"
#include "AvFrameAdapters.hpp"
#include "AvPacketAdapters.hpp"
#include "BaseAvCodecContextProperties.hpp"
#include "ErrorWithContext.hpp"

namespace vehlwn::ffmpeg::detail {
class ScopedEncoderContext : public BaseAvCodecContextProperties {
    const AVCodec* m_codec = nullptr;
    AVCodecContext* m_raw = nullptr;

public:
    explicit ScopedEncoderContext(const AVCodec* const codec)
        : m_codec(codec)
        , m_raw(avcodec_alloc_context3(codec))
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
        std::swap(m_raw, rhs.m_raw);
        std::swap(m_codec, rhs.m_codec);
    }

    void send_frame(const OwningAvframe& frame) const
    {
        send_frame_impl(frame.raw());
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
