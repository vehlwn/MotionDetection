#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class ResizeFilter : public IImageFilter {
public:
    ResizeFilter(double scale_factor);
    virtual CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;

private:
    const double m_scale_factor;
};
} // namespace vehlwn
