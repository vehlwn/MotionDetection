#pragma once

#include "ISmoothingFilter.h"

namespace vehlwn {
class IdentityFilter : public ISmoothingFilter
{
public:
    virtual cv::Mat apply(const cv::Mat& input) override;
};
} // namespace vehlwn
