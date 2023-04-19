#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class IdentityFilter : public IImageFilter {
public:
    CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;
};
} // namespace vehlwn
