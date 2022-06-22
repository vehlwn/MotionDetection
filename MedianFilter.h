#pragma once

#include "ISmoothingFilter.h"

namespace vehlwn {
class MedianFilter : public ISmoothingFilter
{
public:
    MedianFilter(int kernel_size);
    virtual cv::Mat apply(const cv::Mat& input) override;

private:
    int m_kernel_size;
};
} // namespace vehlwn
