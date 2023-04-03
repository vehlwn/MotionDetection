#pragma once

#include <memory>
#include <string>

#include "ApplicationSettings.hpp"
#include "DemuxerOptionsFactory.hpp"
#include "ffmpeg/InputDevice.hpp"

namespace vehlwn {
class FfmpegInputDeviceFactory {
    const ApplicationSettings& m_settings;

public:
    FfmpegInputDeviceFactory(const ApplicationSettings& settings)
        : m_settings(settings)
    {}
    ffmpeg::InputDevice create()
    {
        auto demuxer_options
            = DemuxerOptionsFactory().create_options(m_settings.video_capture);
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
