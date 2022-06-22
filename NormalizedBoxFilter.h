#pragma once

#include "IImageFilter.h"

namespace vehlwn {
class NormalizedBoxFilter : public IImageFilter
{
public:
    NormalizedBoxFilter(int kernel_size);
    virtual cv::Mat apply(const cv::Mat& input) override;

private:
    const int m_kernel_size;
};
} // namespace vehlwn
