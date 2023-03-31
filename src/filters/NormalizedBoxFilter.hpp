#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class NormalizedBoxFilter : public IImageFilter {
public:
    NormalizedBoxFilter(int kernel_size);
    virtual CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;

private:
    const int m_kernel_size;
};
} // namespace vehlwn
