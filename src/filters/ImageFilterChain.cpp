#include "ImageFilterChain.hpp"

#include <algorithm>

namespace vehlwn {

void ImageFilterChain::add_filter(std::shared_ptr<IImageFilter> filter)
{
    m_filters.push_back(std::move(filter));
}

cv::Mat ImageFilterChain::apply(const cv::Mat& input)
{
    cv::Mat ret = input.clone();
    std::for_each(m_filters.cbegin(), m_filters.cend(), [&ret](const auto& filter) {
        ret = filter->apply(ret);
    });
    return ret;
}

} // namespace vehlwn
