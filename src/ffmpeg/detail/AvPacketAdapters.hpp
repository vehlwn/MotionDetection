#pragma once

#include <stdexcept>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/rational.h>
}

namespace vehlwn::ffmpeg::detail {
class OwningAvPacket {
    AVPacket* m_raw = nullptr;

    void alloc()
    {
        m_raw = av_packet_alloc();
        if(!m_raw)
            throw std::runtime_error("failed to allocate memory for AVPacket");
    }

public:
    OwningAvPacket()
    {
        alloc();
    }
    OwningAvPacket(const OwningAvPacket&) = delete;
    OwningAvPacket(OwningAvPacket&& rhs) noexcept
    {
        alloc();
        av_packet_move_ref(m_raw, rhs.m_raw);
    }
    ~OwningAvPacket()
    {
        av_packet_free(&m_raw);
    }
    const AVPacket* raw() const
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
    int stream_index() const
    {
        return m_raw->stream_index;
    }
    std::int64_t pts() const
    {
        return m_raw->pts;
    }
    std::int64_t dts() const
    {
        return m_raw->dts;
    }
};
} // namespace vehlwn::ffmpeg::detail
