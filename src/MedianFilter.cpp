#include "MedianFilter.hpp"

#include "opencv2/imgproc.hpp"

namespace vehlwn {
MedianFilter::MedianFilter(int kernel_size)
    : m_kernel_size{kernel_size} {
}

cv::Mat MedianFilter::apply(const cv::Mat& input) {
    cv::Mat ret;
    cv::medianBlur(input, ret, m_kernel_size);
    return ret;
}
} // namespace vehlwn
