#pragma once

#include <memory>
#include <optional>
#include <string>

#include <opencv2/core/mat.hpp>

#include "CvMatRaiiAdapter.hpp"
#include "ffmpeg/ScopedAvDictionary.hpp"

namespace vehlwn::ffmpeg {
class InputDevice {
public:
    struct Impl;
    InputDevice(std::unique_ptr<Impl>&& pimpl);

    InputDevice(const InputDevice&) = delete;
    InputDevice(InputDevice&&);
    ~InputDevice();

    CvMatRaiiAdapter get_video_frame() const;
    double fps() const;
    void start_recording(const char* const path) const;
    void stop_recording() const;
    bool is_recording() const;

    void set_out_video_bitrate(std::optional<std::string>&& x) const;
    void set_out_audio_bitrate(std::optional<std::string>&& x) const;

private:
    std::unique_ptr<Impl> pimpl;
};

InputDevice open_input_device(
    const char* const url,
    const std::optional<std::string>& file_format,
    ScopedAvDictionary& options);
} // namespace vehlwn::ffmpeg
