#include "GaussianBlurFilter.hpp"

#include <opencv2/imgproc.hpp>

namespace vehlwn {
GaussianBlurFilter::GaussianBlurFilter(int kernel_size, double sigma)
    : m_kernel_size{kernel_size}
    , m_sigma{sigma}
{}

CvMatRaiiAdapter GaussianBlurFilter::apply(CvMatRaiiAdapter&& input)
{
    cv::GaussianBlur(
        input.get(),
        input.get(),
        cv::Size{m_kernel_size, m_kernel_size},
        m_sigma,
        m_sigma);
    return std::move(input);
}
} // namespace vehlwn
