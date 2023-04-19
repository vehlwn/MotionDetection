#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class MedianFilter : public IImageFilter {
public:
    explicit MedianFilter(int kernel_size);
    CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;

private:
    const int m_kernel_size;
};
} // namespace vehlwn
