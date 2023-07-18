#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "BackgroundSubtractorFactory.hpp"
#include "FileNameFactory.hpp"
#include "MotionData.hpp"
#include "PreprocessImageFactory.hpp"
#include "SharedMutex.hpp"
#include "ffmpeg_adapters/InputDevice.hpp"

namespace vehlwn {
class MotionDataWorker {
public:
    explicit MotionDataWorker(
        std::shared_ptr<const vehlwn::ApplicationSettings>&& settings);
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
    [[nodiscard]] bool is_recording() const;

private:
    std::shared_ptr<BackgroundSubtractorFactory> m_back_subtractor_factory;
    ffmpeg::InputDevice m_input_device;
    std::shared_ptr<FileNameFactory> m_out_filename_factory;
    std::shared_ptr<PreprocessImageFactory> m_preprocess_image_factory;
    std::shared_ptr<const vehlwn::ApplicationSettings> m_settings;

    std::shared_ptr<SharedMutex<MotionData>> m_motion_data;
    std::atomic_bool m_stopped;

    std::thread m_working_thread;

    void thread_func(
        std::shared_ptr<IBackgroundSubtractor>&& back_subtractor,
        std::shared_ptr<IImageFilter>&& preprocess_filter);
};
} // namespace vehlwn
