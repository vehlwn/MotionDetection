#include "GaussianBlurFilter.h"

#include "opencv2/imgproc.hpp"

namespace vehlwn {
GaussianBlurFilter::GaussianBlurFilter(int kernel_size, double sigma)
    : m_kernel_size{kernel_size}
    , m_sigma{sigma}
{
}

cv::Mat GaussianBlurFilter::apply(const cv::Mat& input)
{
    cv::Mat ret;
    cv::GaussianBlur(
        input,
        ret,
        cv::Size{m_kernel_size, m_kernel_size},
        m_sigma,
        m_sigma);
    return ret;
}
} // namespace vehlwn
