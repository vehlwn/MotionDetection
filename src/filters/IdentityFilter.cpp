#include "IdentityFilter.hpp"

namespace vehlwn {
CvMatRaiiAdapter IdentityFilter::apply(CvMatRaiiAdapter&& input)
{
    return std::move(input);
}
} // namespace vehlwn
