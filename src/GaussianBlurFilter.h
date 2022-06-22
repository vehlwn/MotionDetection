#pragma once

#include "IImageFilter.h"

namespace vehlwn {
class GaussianBlurFilter : public IImageFilter
{
public:
    GaussianBlurFilter(int kernel_size, double sigma);
    virtual cv::Mat apply(const cv::Mat& input) override;

private:
    const int m_kernel_size;
    const double m_sigma;
};
} // namespace vehlwn
