#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include <Poco/Logger.h>

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
        std::shared_ptr<BackgroundSubtractorFactory> &&back_subtractor_factory,
        ffmpeg::InputDevice &&input_device,
        std::shared_ptr<FileNameFactory> &&out_filename_factory,
        std::shared_ptr<PreprocessImageFactory> &&preprocess_image_factory,
        Poco::Logger& logger);
    ~MotionDataWorker();
    std::shared_ptr<const SharedMutex<MotionData>> get_motion_data() const;
    void start();
    void stop();
    double get_fps() const;

private:
    std::shared_ptr<BackgroundSubtractorFactory> m_back_subtractor_factory;
    ffmpeg::InputDevice m_input_device;
    std::shared_ptr<FileNameFactory> m_out_filename_factory;
    std::shared_ptr<PreprocessImageFactory> m_preprocess_image_factory;
    std::shared_ptr<SharedMutex<MotionData>> m_motion_data;
    std::atomic_bool m_stopped;
    Poco::Logger& m_logger;

    std::thread m_working_thread;
};
} // namespace vehlwn
