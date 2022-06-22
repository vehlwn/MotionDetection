#pragma once

#include "ISmoothingFilter.h"

namespace vehlwn {
class NormalizedBoxFilter : public ISmoothingFilter
{
public:
    NormalizedBoxFilter(int kernel_size);
    virtual cv::Mat apply(const cv::Mat& input) override;

private:
    int m_kernel_size;
};
} // namespace vehlwn
