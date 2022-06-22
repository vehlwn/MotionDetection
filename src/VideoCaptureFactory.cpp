#include "VideoCaptureFactory.hpp"

#include "IVideoCapture.hpp"
#include "OpencvVideoCapture.hpp"
#include "fmt/core.h"
#include "opencv2/videoio.hpp"

namespace vehlwn {
VideoCaptureFactory::VideoCaptureFactory(
    std::shared_ptr<ApplicationSettings> config,
    Poco::Logger& logger)
    : m_config{std::move(config)}
    , m_logger{logger}
{}

std::shared_ptr<IVideoCapture> VideoCaptureFactory::create()
{
    const auto ret = std::make_shared<OpencvVideoCapture>(m_logger);
    const std::string filename = m_config->get_video_capture_filename();
    poco_information(m_logger, fmt::format("video_capture.filename = {}", filename));
    int api_preference = cv::VideoCaptureAPIs::CAP_ANY;
    const std::string tmp_api_preference_str
        = m_config->get_video_capture_api_preference("CAP_V4L2");
    poco_information(
        m_logger,
        fmt::format("video_capture.api_preference = {}", tmp_api_preference_str));
    if(tmp_api_preference_str == "CAP_FFMPEG")
        api_preference = cv::VideoCaptureAPIs::CAP_FFMPEG;
    else if(tmp_api_preference_str == "CAP_V4L2")
        api_preference = cv::VideoCaptureAPIs::CAP_V4L2;
    else {
        poco_information(
            m_logger,
            fmt::format(
                "Unknown api_preference: {}. Choosing default CAP_ANY",
                tmp_api_preference_str));
    }
    ret->open(filename, api_preference);

    if(m_config->has_video_capture_fourcc()) {
        const std::string fourcc = m_config->get_video_capture_fourcc();
        poco_information(m_logger, fmt::format("video_capture.fourcc = {}", fourcc));
        ret->set_fourcc(fourcc);
    }
    if(m_config->has_video_capture_frame_width()) {
        const int frame_width = m_config->get_video_capture_frame_width();
        poco_information(
            m_logger,
            fmt::format("video_capture.frame_width = {}", frame_width));
        ret->set_frame_width(frame_width);
    }
    if(m_config->has_video_capture_frame_height()) {
        const int frame_height = m_config->get_video_capture_frame_height();
        poco_information(
            m_logger,
            fmt::format("video_capture.frame_height = {}", frame_height));
        ret->set_frame_height(frame_height);
    }
    if(m_config->has_video_capture_fps()) {
        const double fps = m_config->get_video_capture_fps();
        poco_information(m_logger, fmt::format("video_capture.fps = {}", fps));
        ret->set_fps(fps);
    }
    return std::move(ret);
}

} // namespace vehlwn
