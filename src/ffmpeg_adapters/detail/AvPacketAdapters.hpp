#pragma once

#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <utility>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/rational.h>
}

namespace vehlwn::ffmpeg::detail {
class OwningAvPacket {
    AVPacket* m_raw = nullptr;

public:
    OwningAvPacket()
        : m_raw(av_packet_alloc())
    {
        if(m_raw == nullptr) {
            throw std::runtime_error("failed to allocate memory for AVPacket");
        }
    }
    OwningAvPacket(const OwningAvPacket&) = delete;
    OwningAvPacket(OwningAvPacket&& rhs) noexcept
    {
        swap(rhs);
    }
    ~OwningAvPacket()
    {
        av_packet_free(&m_raw);
    }
    OwningAvPacket& operator=(const OwningAvPacket&) = delete;
    OwningAvPacket& operator=(OwningAvPacket&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }
    void swap(OwningAvPacket& rhs) noexcept
    {
        std::swap(m_raw, rhs.m_raw);
    }

    [[nodiscard]] const AVPacket* raw() const
    {
        return m_raw;
    }
    AVPacket* raw()
    {
        return m_raw;
    }
    void set_stream_index(const int x) const
    {
        m_raw->stream_index = x;
    }
    [[nodiscard]] int stream_index() const
    {
        return m_raw->stream_index;
    }
    [[nodiscard]] std::int64_t pts() const
    {
        return m_raw->pts;
    }
    [[nodiscard]] std::int64_t dts() const
    {
        return m_raw->dts;
    }
};
} // namespace vehlwn::ffmpeg::detail
