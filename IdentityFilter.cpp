#include "IdentityFilter.h"

namespace vehlwn {
cv::Mat IdentityFilter::apply(const cv::Mat& input)
{
    return input;
}
} // namespace vehlwn
