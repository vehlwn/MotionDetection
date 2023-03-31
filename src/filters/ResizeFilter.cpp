#include "ResizeFilter.hpp"

#include <opencv2/imgproc.hpp>

namespace vehlwn {
ResizeFilter::ResizeFilter(double scale_factor)
    : m_scale_factor{scale_factor}
{}

CvMatRaiiAdapter ResizeFilter::apply(CvMatRaiiAdapter&& input)
{
    cv::resize(
        input.get(),
        input.get(),
        cv::Size{0, 0},
        m_scale_factor,
        m_scale_factor);
    return std::move(input);
}
} // namespace vehlwn
