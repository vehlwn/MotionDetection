#pragma once

#include <cstdint>
#include <vector>

extern "C" {
#include <libavutil/mem.h>
#include <libavutil/samplefmt.h>
}

#include "../ErrorWithContext.hpp"
#include "AvError.hpp"

namespace vehlwn::ffmpeg::detail {
class ScopedAvSAmplesBuffer {
    std::vector<std::uint8_t*> m_raw;

public:
    ScopedAvSAmplesBuffer(
        const int channels,
        const int frame_size,
        const AVSampleFormat sample_fmt)
    {
        m_raw.resize(static_cast<std::size_t>(channels));
        const int errnum = av_samples_alloc(
            m_raw.data(),
            nullptr,
            channels,
            frame_size,
            sample_fmt,
            0);
        if(errnum < 0) {
            throw ErrorWithContext("av_samples_alloc failed: ", AvError(errnum));
        }
    }
    ScopedAvSAmplesBuffer(const ScopedAvSAmplesBuffer&) = delete;
    ScopedAvSAmplesBuffer(ScopedAvSAmplesBuffer&& rhs) noexcept
    {
        swap(rhs);
    }
    ~ScopedAvSAmplesBuffer()
    {
        if(!m_raw.empty()) {
            av_freep(m_raw.data());
        }
    }
    ScopedAvSAmplesBuffer& operator=(const ScopedAvSAmplesBuffer&) = delete;
    ScopedAvSAmplesBuffer& operator=(ScopedAvSAmplesBuffer&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }
    void swap(ScopedAvSAmplesBuffer& rhs) noexcept
    {
        std::swap(m_raw, rhs.m_raw);
    }

    std::uint8_t** data()
    {
        return m_raw.data();
    }
};
} // namespace vehlwn::ffmpeg::detail
