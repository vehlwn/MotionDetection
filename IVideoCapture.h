#pragma once

#include "opencv2/core/mat.hpp"

#include <optional>
#include <string>

namespace vehlwn {
class IVideoCapture
{
public:
    virtual ~IVideoCapture() = default;
    virtual std::optional<cv::Mat> read() = 0;
};
} // namespace vehlwn
