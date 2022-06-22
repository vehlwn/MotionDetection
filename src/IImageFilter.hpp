#pragma once

#include "opencv2/core/mat.hpp"

namespace vehlwn {
class IImageFilter {
public:
    IImageFilter() = default;
    virtual cv::Mat apply(const cv::Mat& input) = 0;
};
} // namespace vehlwn
