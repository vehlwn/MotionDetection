#pragma once

#include "IImageFilter.hpp"

namespace vehlwn {
class IdentityFilter : public IImageFilter {
public:
    virtual CvMatRaiiAdapter apply(CvMatRaiiAdapter&& input) override;
};
} // namespace vehlwn
