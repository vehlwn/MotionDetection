#pragma once

#include <stdexcept>

extern "C" {
#include <libavutil/audio_fifo.h>
}

#include "AvError.hpp"
#include "ErrorWithContext.hpp"

namespace vehlwn::ffmpeg::detail {
class ScopedAvAudioFifo {
    AVAudioFifo* m_raw = nullptr;

public:
    ScopedAvAudioFifo(const AVSampleFormat sample_fmt, const int channels)
    {
        m_raw = av_audio_fifo_alloc(sample_fmt, channels, 1);
        if(!m_raw)
            throw std::runtime_error("Could not allocate AVAudioFifo");
    }
    ScopedAvAudioFifo(const ScopedAvAudioFifo&) = delete;
    ScopedAvAudioFifo(ScopedAvAudioFifo&& rhs) noexcept
    {
        swap(rhs);
    }
    ~ScopedAvAudioFifo()
    {
        av_audio_fifo_free(m_raw);
    }
    void swap(ScopedAvAudioFifo& rhs) noexcept
    {
        std::swap(m_raw, rhs.m_raw);
    }

    int size() const
    {
        return av_audio_fifo_size(m_raw);
    }

    void add_samples(std::uint8_t** const data, const int frame_size) const
    {
        int errnum = 0;
        errnum = av_audio_fifo_realloc(m_raw, size() + frame_size);
        if(errnum < 0)
            throw ErrorWithContext(
                "Could not reallocate AVAudioFifo: ",
                AvError(errnum));
        errnum
            = av_audio_fifo_write(m_raw, reinterpret_cast<void**>(data), frame_size);
        if(errnum < 0)
            throw ErrorWithContext(
                "Could not write data to AVAudioFifo: ",
                AvError(errnum));
    }

    void read(void** data, const int nb_samples) const
    {
        const int errnum = av_audio_fifo_read(m_raw, data, nb_samples);
        if(errnum < 0)
            throw ErrorWithContext("av_audio_fifo_read failed: ", AvError(errnum));
    }
};
} // namespace vehlwn::ffmpeg::detail
