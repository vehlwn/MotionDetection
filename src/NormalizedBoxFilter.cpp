#include "NormalizedBoxFilter.h"

#include "opencv2/imgproc.hpp"

namespace vehlwn {
NormalizedBoxFilter::NormalizedBoxFilter(int kernel_size)
    : m_kernel_size{kernel_size}
{
}

cv::Mat NormalizedBoxFilter::apply(const cv::Mat& input)
{
    cv::Mat ret;
    cv::blur(input, ret, {m_kernel_size, m_kernel_size});
    return ret;
}
} // namespace vehlwn
