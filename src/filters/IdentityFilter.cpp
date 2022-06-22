#include "IdentityFilter.hpp"

namespace vehlwn {
cv::Mat IdentityFilter::apply(const cv::Mat& input) {
    return input.clone();
}
} // namespace vehlwn
