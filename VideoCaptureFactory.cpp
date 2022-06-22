#include "VideoCaptureFactory.h"

#include "IVideoCapture.h"
#include "OpencvVideoCapture.h"
#include "fmt/core.h"
#include "opencv2/videoio.hpp"

namespace vehlwn {
VideoCaptureFactory::VideoCaptureFactory(
    const Poco::Util::AbstractConfiguration& config,
    Poco::Logger& logger)
    : m_config{config}
    , m_logger{logger}
{
}

std::shared_ptr<IVideoCapture> VideoCaptureFactory::create()
{
    const auto ret = std::make_shared<OpencvVideoCapture>(m_logger);
    const std::string filename = m_config.getString("video_capture.filename");
    poco_information(m_logger, fmt::format("video_capture.filename = {}", filename));
    int api_preference = cv::VideoCaptureAPIs::CAP_ANY;
    const std::string tmp_api_preference_str =
        m_config.getString("video_capture.api_preference");
    poco_information(
        m_logger,
        fmt::format("video_capture.api_preference = {}", tmp_api_preference_str));
    if(tmp_api_preference_str == "CAP_FFMPEG")
        api_preference = cv::VideoCaptureAPIs::CAP_FFMPEG;
    else if(tmp_api_preference_str == "CAP_V4L2")
        api_preference = cv::VideoCaptureAPIs::CAP_V4L2;
    else
    {
        poco_notice(
            m_logger,
            fmt::format(
                "Unknown api_preference: {}. Choosing default CAP_ANY",
                tmp_api_preference_str));
    }
    ret->open(filename, api_preference);

    const std::string fourcc = m_config.getString("video_capture.fourcc");
    poco_information(m_logger, fmt::format("video_capture.fourcc = {}", fourcc));
    ret->set_fourcc(fourcc);
    const int frame_width = m_config.getInt("video_capture.frame_width");
    poco_information(
        m_logger,
        fmt::format("video_capture.frame_width = {}", frame_width));
    ret->set_frame_width(frame_width);
    const int frame_height = m_config.getInt("video_capture.frame_height");
    poco_information(
        m_logger,
        fmt::format("video_capture.frame_height = {}", frame_height));
    ret->set_frame_height(frame_height);
    const double fps = m_config.getDouble("video_capture.fps");
    poco_information(m_logger, fmt::format("video_capture.fps = {}", fps));
    ret->set_fps(fps);
    return std::move(ret);
}

} // namespace vehlwn
