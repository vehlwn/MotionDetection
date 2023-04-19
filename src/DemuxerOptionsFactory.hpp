#pragma once

#include "ApplicationSettings.hpp"
#include "ffmpeg/ScopedAvDictionary.hpp"

namespace vehlwn {
class DemuxerOptionsFactory {
    const ApplicationSettings::VideoCapture& m_settings;

public:
    explicit DemuxerOptionsFactory(const ApplicationSettings::VideoCapture& capture)
        : m_settings(capture)
    {}
    [[nodiscard]] ffmpeg::ScopedAvDictionary create_options() const
    {
        ffmpeg::ScopedAvDictionary ret;
        if(m_settings.video_size) {
            ret.set_str("video_size", m_settings.video_size.value().data());
        }
        if(m_settings.framerate) {
            ret.set_str("framerate", m_settings.framerate.value().data());
        }
        if(m_settings.input_format) {
            ret.set_str("input_format", m_settings.input_format.value().data());
        }
        return ret;
    }
};
} // namespace vehlwn
