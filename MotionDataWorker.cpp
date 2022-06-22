#include "MotionDataWorker.h"

#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <cstdlib>
#include <memory>

namespace vehlwn {
MotionDataWorker::MotionDataWorker(
    std::shared_ptr<OpencvBackgroundSubtractorFactory> back_subtractor_factory,
    std::shared_ptr<VideoCaptureFactory> video_capture_factory,
    Poco::Logger& logger)
    : m_back_subtractor_factory{std::move(back_subtractor_factory)}
    , m_video_capture_factory{std::move(video_capture_factory)}
    , m_motion_data{std::make_shared<Mutex<MotionData>>(MotionData{{}, {}})}
    , m_stopped{false}
    , m_logger{logger}
{
    poco_information(m_logger, "constructor MotionDataWorker");
}

MotionDataWorker::~MotionDataWorker()
{
    poco_information(m_logger, "destructor ~MotionDataWorker");
    if(!m_stopped)
        stop();
}

const std::shared_ptr<const Mutex<MotionData>>
    MotionDataWorker::get_motion_data() const
{
    return m_motion_data;
}

void MotionDataWorker::start()
{
    m_stopped = false;
    std::shared_ptr<cv::BackgroundSubtractor> back_subtractor =
        m_back_subtractor_factory->create();
    m_video_capture = m_video_capture_factory->create();
    m_working_thread = std::thread{[this, back_subtractor] {
        while(!m_stopped)
        {
            std::optional<cv::Mat> opt_frame = m_video_capture->read();
            if(!opt_frame)
            {
                poco_fatal(m_logger, "Cannot read more frames from a capture file");
                std::abort();
            }
            cv::Mat frame = std::move(*opt_frame);
            cv::Mat fgmask;
            back_subtractor->apply(frame, fgmask);
            *m_motion_data->lock() = {std::move(frame), std::move(fgmask)};
        }
    }};
}

void MotionDataWorker::stop()
{
    poco_information(m_logger, "Stopping...");
    m_stopped = true;
    if(m_working_thread.joinable())
    {
        poco_information(m_logger, "Joining working thread...");
        m_working_thread.join();
        poco_information(m_logger, "Joined");
    }
    *m_motion_data->lock() = {{}, {}};
    m_video_capture = nullptr;
}

double MotionDataWorker::get_fps() const
{
    if(!m_stopped && m_video_capture)
        return m_video_capture->get_fps();
    else
    {
        poco_error(m_logger, "Called get_fps() on stopped video_capture object!");
        return 0.0;
    }
}
} // namespace vehlwn
