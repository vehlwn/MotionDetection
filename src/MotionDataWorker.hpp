#pragma once

#include "MotionData.hpp"
#include "Mutex.hpp"
#include "OpencvBackgroundSubtractorFactory.hpp"
#include "Poco/Logger.h"
#include "PreprocessImageFactory.hpp"
#include "VideoCaptureFactory.hpp"

#include <atomic>
#include <memory>
#include <thread>

namespace vehlwn {
class MotionDataWorker
{
public:
    MotionDataWorker(
        std::shared_ptr<OpencvBackgroundSubtractorFactory> back_subtractor_factory,
        std::shared_ptr<VideoCaptureFactory> video_capture_factory,
        std::shared_ptr<PreprocessImageFactory> smoothing_filter_factory,
        Poco::Logger& logger);
    ~MotionDataWorker();
    std::shared_ptr<const Mutex<MotionData>> get_motion_data() const;
    void start();
    void stop();
    double get_fps() const;

private:
    std::shared_ptr<OpencvBackgroundSubtractorFactory> m_back_subtractor_factory;
    std::shared_ptr<VideoCaptureFactory> m_video_capture_factory;
    std::shared_ptr<PreprocessImageFactory> m_preprocess_image_factory;
    std::shared_ptr<Mutex<MotionData>> m_motion_data;
    std::atomic_bool m_stopped;
    Poco::Logger& m_logger;

    std::shared_ptr<IVideoCapture> m_video_capture;
    std::thread m_working_thread;
};
} // namespace vehlwn
