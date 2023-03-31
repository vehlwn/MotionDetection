#include "MedianFilter.hpp"

#include <opencv2/imgproc.hpp>

namespace vehlwn {
MedianFilter::MedianFilter(int kernel_size)
    : m_kernel_size{kernel_size}
{}

CvMatRaiiAdapter MedianFilter::apply(CvMatRaiiAdapter&& input)
{
    cv::medianBlur(input.get(), input.get(), m_kernel_size);
    return std::move(input);
}
} // namespace vehlwn
