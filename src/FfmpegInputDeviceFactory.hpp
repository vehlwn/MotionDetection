#pragma once

#include <memory>
#include <string>

#include "ApplicationSettings.hpp"
#include "ffmpeg_adapters/InputDevice.hpp"
#include "ffmpeg_adapters/ScopedAvDictionary.hpp"

namespace vehlwn {
class FfmpegInputDeviceFactory {
    const ApplicationSettings& m_settings;

public:
    explicit FfmpegInputDeviceFactory(const ApplicationSettings& settings)
        : m_settings(settings)
    {}
    ffmpeg::InputDevice create()
    {
        auto demuxer_options = ffmpeg::ScopedAvDictionary::from_std_map(
            m_settings.video_capture.demuxer_options);
        auto ret = vehlwn::ffmpeg::open_input_device(
            m_settings.video_capture.filename.data(),
            m_settings.video_capture.file_format,
            demuxer_options);
        ret.set_out_video_bitrate(
            std::optional(m_settings.output_files.video_bitrate));
        ret.set_out_audio_bitrate(
            std::optional(m_settings.output_files.audio_bitrate));
        return ret;
    }
};
} // namespace vehlwn
