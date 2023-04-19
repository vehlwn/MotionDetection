#pragma once

#include <exception>
#include <iostream>
#include <stdexcept>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/rational.h>
}

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

namespace vehlwn::ffmpeg::detail {
class OwningAvPacket {
    AVPacket* m_raw = nullptr;

    void alloc()
    {
        m_raw = av_packet_alloc();
        if(m_raw == nullptr) {
            throw std::runtime_error("failed to allocate memory for AVPacket");
        }
    }

public:
    OwningAvPacket()
    {
        alloc();
    }
    OwningAvPacket(const OwningAvPacket&) = delete;
    OwningAvPacket(OwningAvPacket&& rhs) noexcept
    try {
        BOOST_LOG_FUNCTION();
        alloc();
        av_packet_move_ref(m_raw, rhs.m_raw);
    } catch(const std::exception& ex) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
    }
    ~OwningAvPacket()
    {
        av_packet_free(&m_raw);
    }
    OwningAvPacket& operator=(const OwningAvPacket&) = delete;
    OwningAvPacket& operator=(OwningAvPacket&& rhs) noexcept
    {
        av_packet_move_ref(m_raw, rhs.m_raw);
        return *this;
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
