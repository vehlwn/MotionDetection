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
    explicit InputDevice(std::unique_ptr<Impl>&& pimpl);

    InputDevice(const InputDevice&) = delete;
    InputDevice(InputDevice&&) noexcept ;
    ~InputDevice();
    InputDevice& operator=(const InputDevice&) = delete;
    InputDevice& operator=(InputDevice&&) noexcept;

    [[nodiscard]] CvMatRaiiAdapter get_video_frame() const;
    [[nodiscard]] double fps() const;
    void start_recording(const char*  path) const;
    void stop_recording() const;
    [[nodiscard]] bool is_recording() const;

    void set_out_video_bitrate(std::optional<std::string>&& x) const;
    void set_out_audio_bitrate(std::optional<std::string>&& x) const;

private:
    std::unique_ptr<Impl> pimpl;
};

InputDevice open_input_device(
    const char*  url,
    const std::optional<std::string>& file_format,
    ScopedAvDictionary& options);
} // namespace vehlwn::ffmpeg
