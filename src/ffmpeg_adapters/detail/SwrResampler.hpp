#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>

extern "C" {
#include <libswresample/swresample.h>
}

#include "../ErrorWithContext.hpp"
#include "AvError.hpp"

namespace vehlwn::ffmpeg::detail {
class ScopedSwrResampler {
    friend class SwrResamplerBuiler;
    SwrContext* m_raw = nullptr;

    explicit ScopedSwrResampler(SwrContext* raw)
        : m_raw(raw)
    {}

public:
    ScopedSwrResampler(const ScopedSwrResampler&) = delete;
    ScopedSwrResampler(ScopedSwrResampler&& rhs) noexcept
    {
        swap(rhs);
    }
    ~ScopedSwrResampler()
    {
        swr_free(&m_raw);
    }
    ScopedSwrResampler& operator=(const ScopedSwrResampler&) = delete;
    ScopedSwrResampler& operator=(ScopedSwrResampler&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }
    void swap(ScopedSwrResampler& rhs) noexcept
    {
        std::swap(m_raw, rhs.m_raw);
    }

    void convert(
        const std::uint8_t** const input_data,
        std::uint8_t** const converted_data,
        const int frame_size) const
    {
        const int errnum
            = swr_convert(m_raw, converted_data, frame_size, input_data, frame_size);
        if(errnum < 0) {
            throw ErrorWithContext("swr_convert failed: ", AvError(errnum));
        }
    }
};

class SwrResamplerBuiler {
    const AVChannelLayout* m_in_ch_layout = nullptr;
    std::optional<AVSampleFormat> m_in_sample_fmt;
    std::optional<int> m_in_sample_rate;
    const AVChannelLayout* m_out_ch_layout = nullptr;
    std::optional<AVSampleFormat> m_out_sample_fmt;
    std::optional<int> m_out_sample_rate;

public:
    SwrResamplerBuiler& in_ch_layout(const AVChannelLayout* const x)
    {
        m_in_ch_layout = x;
        return *this;
    }
    SwrResamplerBuiler& in_sample_fmt(const AVSampleFormat x)
    {
        m_in_sample_fmt = x;
        return *this;
    }
    SwrResamplerBuiler& in_sample_rate(const int x)
    {
        m_in_sample_rate = x;
        return *this;
    }
    SwrResamplerBuiler& out_ch_layout(const AVChannelLayout* const x)
    {
        m_out_ch_layout = x;
        return *this;
    }
    SwrResamplerBuiler& out_sample_fmt(const AVSampleFormat x)
    {
        m_out_sample_fmt = x;
        return *this;
    }
    SwrResamplerBuiler& out_sample_rate(const int x)
    {
        m_out_sample_rate = x;
        return *this;
    }
    ScopedSwrResampler build()
    {
        if(m_in_ch_layout == nullptr) {
            throw std::runtime_error("in_ch_layout is not set");
        }
        if(!m_in_sample_fmt) {
            throw std::runtime_error("in_sample_fmt is not set");
        }
        if(!m_in_sample_rate) {
            throw std::runtime_error("in_sample_rate is not set");
        }
        if(m_out_ch_layout == nullptr) {
            throw std::runtime_error("out_ch_layout is not set");
        }
        if(!m_out_sample_fmt) {
            throw std::runtime_error("out_sample_fmt is not set");
        }
        if(!m_out_sample_rate) {
            throw std::runtime_error("out_sample_rate is not set");
        }

        SwrContext* raw = nullptr;
        int errnum = swr_alloc_set_opts2(
            &raw,
            m_out_ch_layout,
            m_out_sample_fmt.value(),
            m_out_sample_rate.value(),
            m_in_ch_layout,
            m_in_sample_fmt.value(),
            m_in_sample_rate.value(),
            0,
            nullptr);
        if(errnum < 0) {
            throw ErrorWithContext("swr_alloc_set_opts2 failed: ", AvError(errnum));
        }

        errnum = swr_init(raw);
        if(errnum < 0) {
            throw ErrorWithContext("swr_init failed: ", AvError(errnum));
        }
        return ScopedSwrResampler(raw);
    }
};
} // namespace vehlwn::ffmpeg::detail
