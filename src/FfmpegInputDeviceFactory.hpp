#pragma once

#include <memory>
#include <optional>
#include <string>

#include "ApplicationSettings.hpp"
#include "ffmpeg_adapters/InputDevice.hpp"
#include "ffmpeg_adapters/ScopedAvDictionary.hpp"

namespace vehlwn {
class FfmpegInputDeviceFactory {
    std::shared_ptr<const ApplicationSettings> m_settings;

public:
    explicit FfmpegInputDeviceFactory(
        std::shared_ptr<const ApplicationSettings>&& settings)
        : m_settings(std::move(settings))
    {}
    ffmpeg::InputDevice create()
    {
        auto ret = vehlwn::ffmpeg::open_input_device(std::shared_ptr(m_settings));
        return ret;
    }
};
} // namespace vehlwn
