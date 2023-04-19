#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class ResizeFilter : public IImageFilter {
public:
    explicit ResizeFilter(double scale_factor);
    CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;

private:
    const double m_scale_factor;
};
} // namespace vehlwn
