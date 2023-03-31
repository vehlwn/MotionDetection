#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class MedianFilter : public IImageFilter {
public:
    MedianFilter(int kernel_size);
    virtual CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;

private:
    const int m_kernel_size;
};
} // namespace vehlwn
