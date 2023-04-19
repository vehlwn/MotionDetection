#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "BackgroundSubtractorFactory.hpp"
#include "FileNameFactory.hpp"
#include "MotionData.hpp"
#include "PreprocessImageFactory.hpp"
#include "SharedMutex.hpp"
#include "ffmpeg/InputDevice.hpp"

namespace vehlwn {
class MotionDataWorker {
public:
    MotionDataWorker(
        std::shared_ptr<BackgroundSubtractorFactory>&& back_subtractor_factory,
        ffmpeg::InputDevice&& input_device,
        std::shared_ptr<FileNameFactory>&& out_filename_factory,
        std::shared_ptr<PreprocessImageFactory>&& preprocess_image_factory);
    MotionDataWorker(const MotionDataWorker&) = delete;
    MotionDataWorker(MotionDataWorker&&) = delete;
    MotionDataWorker& operator=(const MotionDataWorker&) = delete;
    MotionDataWorker& operator=(MotionDataWorker&&) = delete;
    ~MotionDataWorker();
    [[nodiscard]] std::shared_ptr<const SharedMutex<MotionData>>
        get_motion_data() const;
    void start();
    void stop();
    [[nodiscard]] double get_fps() const;

private:
    std::shared_ptr<BackgroundSubtractorFactory> m_back_subtractor_factory;
    ffmpeg::InputDevice m_input_device;
    std::shared_ptr<FileNameFactory> m_out_filename_factory;
    std::shared_ptr<PreprocessImageFactory> m_preprocess_image_factory;
    std::shared_ptr<SharedMutex<MotionData>> m_motion_data;
    std::atomic_bool m_stopped;

    std::thread m_working_thread;
};
} // namespace vehlwn
