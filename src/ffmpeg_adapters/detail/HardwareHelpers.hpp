#pragma once

extern "C" {
#include <libavcodec/codec.h>
#include <libavutil/hwcontext.h>
}

namespace vehlwn::ffmpeg::hw_helpers {
constexpr auto DEFAULT_SW_FORMAT = AV_PIX_FMT_NV12;
constexpr int DEFAULT_HW_POOL_SIZE = 32;

AVHWDeviceType find_hw_device_by_name(const char* name);

AVPixelFormat
    find_hw_pix_fmt(const AVCodec* c, AVHWDeviceType type, bool is_encoder);

AVBufferRef* create_hw_frames_context(
    AVBufferRef* hw_device_ctx,
    int width,
    int height,
    AVPixelFormat hw_format);

} // namespace vehlwn::ffmpeg
