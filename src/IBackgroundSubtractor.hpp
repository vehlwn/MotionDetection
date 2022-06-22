#pragma once

#include <opencv2/core/mat.hpp>

namespace vehlwn {
class IBackgroundSubtractor {
public:
    virtual cv::Mat apply(const cv::Mat& image) = 0;
};
} // namespace vehlwn
