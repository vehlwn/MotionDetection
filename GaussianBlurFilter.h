#pragma once

#include "ISmoothingFilter.h"

namespace vehlwn {
class GaussianBlurFilter : public ISmoothingFilter
{
public:
    GaussianBlurFilter(int kernel_size, double sigma);
    virtual cv::Mat apply(const cv::Mat& input) override;

private:
    int m_kernel_size;
    double m_sigma;
};
} // namespace vehlwn
