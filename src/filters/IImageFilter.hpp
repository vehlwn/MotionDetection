#pragma once

#include <opencv2/core/mat.hpp>

#include "CvMatRaiiAdapter.hpp"

namespace vehlwn {
class IImageFilter {
public:
    IImageFilter() = default;
    IImageFilter(const IImageFilter&) = default;
    IImageFilter(IImageFilter&&) = default;
    IImageFilter& operator=(const IImageFilter&) = default;
    IImageFilter& operator=(IImageFilter&&) = default;
    virtual CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) = 0;
    virtual ~IImageFilter() = default;
};
} // namespace vehlwn
