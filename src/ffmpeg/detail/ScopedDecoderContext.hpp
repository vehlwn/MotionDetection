#pragma once

#include <stdexcept>
#include <variant>

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "AvError.hpp"
#include "AvFrameAdapters.hpp"
#include "AvPacketAdapters.hpp"
#include "BaseAvCodecContextProperties.hpp"
#include "ErrorWithContext.hpp"

namespace vehlwn::ffmpeg::detail {
class ScopedDecoderContext : public BaseAvCodecContextProperties {
public:
    ScopedDecoderContext(
        const AVCodec* const codec,
        const AVCodecParameters* const par)
    {
        m_raw = avcodec_alloc_context3(codec);
        if(!m_raw)
            throw std::runtime_error(
                "Failed to allocated memory for AVCodecContext");
        int errnum = avcodec_parameters_to_context(m_raw, par);
        if(errnum < 0)
            throw ErrorWithContext(
                "avcodec_parameters_to_context failed: ",
                AvError(errnum));
        errnum = avcodec_open2(m_raw, codec, nullptr);
        if(errnum < 0)
            throw ErrorWithContext("avcodec_open2 failed: ", AvError(errnum));
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
    void swap(ScopedDecoderContext& rhs)
    {
        std::swap(m_raw, rhs.m_raw);
    }

    void send_packet(const OwningAvPacket& packet) const
    {
        const int errnum = avcodec_send_packet(m_raw, packet.raw());
        if(errnum < 0)
            throw ErrorWithContext("avcodec_send_packet failed: ", AvError(errnum));
    }

    struct Again {};
    using ReceiveFrameResult = std::variant<OwningAvframe, Again>;
    ReceiveFrameResult receive_frame() const
    {
        OwningAvframe ret;
        const int errnum = avcodec_receive_frame(m_raw, ret.raw());
        if(errnum < 0) {
            if(errnum == AVERROR(EAGAIN))
                return Again{};
            else
                throw ErrorWithContext(
                    "avcodec_receive_frame failed: ",
                    AvError(errnum));
        }
        return ret;
    }
};
} // namespace vehlwn::ffmpeg::detail
