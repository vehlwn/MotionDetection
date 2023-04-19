#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class NormalizedBoxFilter : public IImageFilter {
public:
    explicit NormalizedBoxFilter(int kernel_size);
    CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;

private:
    const int m_kernel_size;
};
} // namespace vehlwn
