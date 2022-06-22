#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class IdentityFilter : public IImageFilter
{
public:
    virtual cv::Mat apply(const cv::Mat& input) override;
};
} // namespace vehlwn
