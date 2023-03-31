#pragma once

#include <opencv2/core/mat.hpp>

#include "CvMatRaiiAdapter.hpp"

namespace vehlwn {
class IImageFilter {
public:
    virtual CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) = 0;
    virtual ~IImageFilter() = default;
};
} // namespace vehlwn
