#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class ConvertToGrayFilter : public IImageFilter {
public:
    CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;
};
} // namespace vehlwn
