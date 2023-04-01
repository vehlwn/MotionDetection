#pragma once

#include "ApplicationSettings.hpp"
#include "ffmpeg/ScopedAvDictionary.hpp"

namespace vehlwn {
class DemuxerOptionsFactory {
public:
    ffmpeg::ScopedAvDictionary
        create_options(const ApplicationSettings::VideoCapture& capture)
    {
        ffmpeg::ScopedAvDictionary ret;
        if(capture.video_size) {
            ret.set_str("video_size", capture.video_size.value().data());
        }
        if(capture.framerate) {
            ret.set_str("framerate", capture.framerate.value().data());
        }
        if(capture.input_format) {
            ret.set_str("input_format", capture.input_format.value().data());
        }
        return ret;
    }
};
} // namespace vehlwn
