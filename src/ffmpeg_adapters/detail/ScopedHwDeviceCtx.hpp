#pragma once

#include <utility>

extern "C" {
#include <libavutil/buffer.h>
}

namespace vehlwn::ffmpeg::detail {
class ScopedHwDeviceCtx {
    AVBufferRef* m_device_ctx = nullptr;

public:
    explicit ScopedHwDeviceCtx(AVBufferRef* p)
        : m_device_ctx(p)
    {}
    ScopedHwDeviceCtx(const ScopedHwDeviceCtx&) = delete;
    ScopedHwDeviceCtx(ScopedHwDeviceCtx&& rhs) noexcept
    {
        swap(rhs);
    }
    ~ScopedHwDeviceCtx()
    {
        av_buffer_unref(&m_device_ctx);
    }
    ScopedHwDeviceCtx& operator=(const ScopedHwDeviceCtx&) = delete;
    ScopedHwDeviceCtx& operator=(ScopedHwDeviceCtx&& rhs) noexcept
    {
        if(this != &rhs) {
            swap(rhs);
        }
        return *this;
    }
    void swap(ScopedHwDeviceCtx& rhs)
    {
        std::swap(m_device_ctx, rhs.m_device_ctx);
    }
};
} // namespace vehlwn::ffmpeg::detail
