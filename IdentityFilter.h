#pragma once

#include "IImageFilter.h"

namespace vehlwn {
class IdentityFilter : public IImageFilter
{
public:
    virtual cv::Mat apply(const cv::Mat& input) override;
};
} // namespace vehlwn
