#pragma once

#include "MotionData.h"
#include "Mutex.h"
#include "OpencvBackgroundSubtractorFactory.h"
#include "Poco/Logger.h"
#include "VideoCaptureFactory.h"

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
        Poco::Logger& logger);
    ~MotionDataWorker();
    const std::shared_ptr<const Mutex<MotionData>> get_motion_data() const;
    void start();

private:
    std::shared_ptr<OpencvBackgroundSubtractorFactory> m_back_subtractor_factory;
    std::shared_ptr<VideoCaptureFactory> m_video_capture_factory;
    std::shared_ptr<Mutex<MotionData>> m_motion_data;
    std::atomic_bool m_stopped;
    Poco::Logger& m_logger;
    std::thread m_working_thread;
};
} // namespace vehlwn