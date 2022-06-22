#pragma once

#include "IVideoCapture.hpp"
#include "Poco/Logger.h"
#include "opencv2/core/mat.hpp"
#include "opencv2/videoio.hpp"

#include <optional>

namespace vehlwn {
class OpencvVideoCapture : public IVideoCapture {
public:
    OpencvVideoCapture(Poco::Logger& logger);
    virtual std::optional<cv::Mat> read() override;
    void open(const std::string& filename, const int api_preference);
    void set_fourcc(const char c1, const char c2, const char c3, const char c4);
    void set_frame_width(const int w);
    void set_frame_height(const int h);
    void set_fps(const double n);
    virtual double get_fps() const override;

private:
    Poco::Logger& m_logger;
    cv::VideoCapture m_cap;
};
} // namespace vehlwn
