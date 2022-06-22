#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class MedianFilter : public IImageFilter
{
public:
    MedianFilter(int kernel_size);
    virtual cv::Mat apply(const cv::Mat& input) override;

private:
    const int m_kernel_size;
};
} // namespace vehlwn
