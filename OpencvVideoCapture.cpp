#include "OpencvVideoCapture.h"

using namespace std::string_literals;

namespace vehlwn {
OpencvVideoCapture::OpencvVideoCapture(Poco::Logger& logger)
    : m_logger{logger}
{
}

void OpencvVideoCapture::open(const std::string& filename, int api_preference)
{
    m_cap.open(filename, api_preference);
    if(!m_cap.isOpened())
    {
        poco_fatal(m_logger, "Unable to open input file: "s + '"' + filename + '"');
    }
}

std::optional<cv::Mat> OpencvVideoCapture::read()
{
    cv::Mat frame;
    const bool ok = m_cap.read(frame);
    if(!ok || frame.empty())
        return std::nullopt;
    return frame;
}

void OpencvVideoCapture::set_fourcc(const std::string& fourcc)
{
    const int decoded_fourcc = cv::VideoWriter::fourcc(
        fourcc.at(0),
        fourcc.at(1),
        fourcc.at(2),
        fourcc.at(3));
    if(!m_cap.set(cv::CAP_PROP_FOURCC, decoded_fourcc))
    {
        poco_warning(m_logger, "Failed to set fourcc for input file");
    }
}

void OpencvVideoCapture::set_frame_width(int w)
{
    if(!m_cap.set(cv::CAP_PROP_FRAME_WIDTH, w))
    {
        poco_warning(m_logger, "Input file does not support custom frame width");
    }
}

void OpencvVideoCapture::set_frame_height(int h)
{
    if(!m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, h))
    {
        poco_warning(m_logger, "Input file does not support custom frame height");
    }
}

void OpencvVideoCapture::set_fps(int n)
{
    if(!m_cap.set(cv::CAP_PROP_FPS, n))
    {
        poco_warning(m_logger, "Input file does not support custom FPS");
    }
}
} // namespace vehlwn
