#include "VideoCaptureFactory.hpp"

#include "IVideoCapture.hpp"
#include "OpencvVideoCapture.hpp"
#include "fmt/core.h"
#include "opencv2/videoio.hpp"

namespace vehlwn {
VideoCaptureFactory::VideoCaptureFactory(
    const ApplicationSettings::VideoCapture& config,
    Poco::Logger& logger)
    : m_config{config}
    , m_logger{logger}
{}

std::shared_ptr<IVideoCapture> VideoCaptureFactory::create()
{
    auto ret = std::make_shared<OpencvVideoCapture>(m_logger);
    int api_preference{};
    switch(m_config.api_preference) {
    case ApplicationSettings::VideoCapture::ApiPreference::CAP_ANY:
        api_preference = cv::VideoCaptureAPIs::CAP_ANY;
        break;
    case ApplicationSettings::VideoCapture::ApiPreference::CAP_FFMPEG:
        api_preference = cv::VideoCaptureAPIs::CAP_FFMPEG;
        break;
    case ApplicationSettings::VideoCapture::ApiPreference::CAP_V4L2:
        api_preference = cv::VideoCaptureAPIs::CAP_V4L2;
        break;
    }
    ret->open(m_config.filename, api_preference);
    if(m_config.fourcc) {
        const auto& fourcc = *m_config.fourcc;
        ret->set_fourcc(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
    }
    if(m_config.size) {
        const auto width = m_config.size->width;
        const auto height = m_config.size->height;
        ret->set_frame_width(width);
        ret->set_frame_height(height);
    }
    if(m_config.framerate) {
        const double fps = *m_config.framerate;
        ret->set_fps(fps);
    }
    return ret;
}

} // namespace vehlwn
