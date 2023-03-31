#include "NormalizedBoxFilter.hpp"

#include <opencv2/imgproc.hpp>

namespace vehlwn {
NormalizedBoxFilter::NormalizedBoxFilter(int kernel_size)
    : m_kernel_size{kernel_size}
{}

CvMatRaiiAdapter NormalizedBoxFilter::apply(CvMatRaiiAdapter&& input)
{
    cv::blur(input.get(), input.get(), {m_kernel_size, m_kernel_size});
    return std::move(input);
}
} // namespace vehlwn
