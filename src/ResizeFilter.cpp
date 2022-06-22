#include "ResizeFilter.hpp"

#include "opencv2/imgproc.hpp"

namespace vehlwn {
ResizeFilter::ResizeFilter(double scale_factor)
    : m_scale_factor{scale_factor} {
}

cv::Mat ResizeFilter::apply(const cv::Mat& input) {
    cv::Mat ret;
    cv::resize(input, ret, cv::Size{0, 0}, m_scale_factor, m_scale_factor);
    return ret;
}
} // namespace vehlwn
