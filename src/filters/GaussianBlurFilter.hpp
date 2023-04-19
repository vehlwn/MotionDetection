#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class GaussianBlurFilter : public IImageFilter {
public:
    GaussianBlurFilter(int kernel_size, double sigma);
    CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;

private:
    const int m_kernel_size;
    const double m_sigma;
};
} // namespace vehlwn
