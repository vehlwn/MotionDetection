#pragma once

#include "opencv2/core/mat.hpp"

namespace vehlwn {
class IImageFilter {
public:
    virtual cv::Mat apply(const cv::Mat& input) = 0;
    virtual ~IImageFilter() = default;
};
} // namespace vehlwn
