#pragma once

#include "IImageFilter.h"

namespace vehlwn {
class ResizeFilter : public IImageFilter
{
public:
    ResizeFilter(double scale_factor);
    virtual cv::Mat apply(const cv::Mat& input) override;

private:
    const double m_scale_factor;
};
} // namespace vehlwn
