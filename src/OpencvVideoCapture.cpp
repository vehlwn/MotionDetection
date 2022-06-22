#include "OpencvVideoCapture.hpp"

#include <cstdlib>
#include <fmt/core.h>

using namespace std::string_literals;

namespace vehlwn {
OpencvVideoCapture::OpencvVideoCapture(Poco::Logger& logger)
    : m_logger{logger}
{}

void OpencvVideoCapture::open(const std::string& filename, const int api_preference)
{
    m_cap.open(filename, api_preference);
    if(!m_cap.isOpened()) {
        poco_fatal(
            m_logger,
            fmt::format("Unable to open input file: \"{}\"", filename));
        std::abort();
    }
}

std::optional<cv::Mat> OpencvVideoCapture::read()
{
    cv::Mat frame;
    const bool ok = m_cap.read(frame);
    if(!ok || frame.empty())
        return std::nullopt;
    return std::optional<cv::Mat>{std::move(frame)};
}

void OpencvVideoCapture::set_fourcc(
    const char c1,
    const char c2,
    const char c3,
    const char c4)
{
    const int decoded_fourcc = cv::VideoWriter::fourcc(c1, c2, c3, c4);
    if(!m_cap.set(cv::CAP_PROP_FOURCC, decoded_fourcc)) {
        poco_warning(m_logger, "Failed to set fourcc for input file");
    }
}

void OpencvVideoCapture::set_frame_width(const int w)
{
    if(!m_cap.set(cv::CAP_PROP_FRAME_WIDTH, w)) {
        poco_warning(m_logger, "Input file does not support custom frame width");
    }
}

void OpencvVideoCapture::set_frame_height(const int h)
{
    if(!m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, h)) {
        poco_warning(m_logger, "Input file does not support custom frame height");
    }
}

void OpencvVideoCapture::set_fps(const double n)
{
    if(!m_cap.set(cv::CAP_PROP_FPS, n)) {
        poco_warning(m_logger, "Input file does not support custom FPS");
    }
}

double OpencvVideoCapture::get_fps() const
{
    const double ret = m_cap.get(cv::CAP_PROP_FPS);
    if(ret == 0.0) {
        poco_warning(m_logger, "CAP_PROP_FPS is not supported");
    }
    return ret;
}
} // namespace vehlwn
