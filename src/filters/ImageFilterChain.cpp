#include "ImageFilterChain.hpp"

#include <boost/range/algorithm/for_each.hpp>

namespace vehlwn {

void ImageFilterChain::add_filter(std::shared_ptr<IImageFilter>&& filter)
{
    m_filters.emplace_back(std::move(filter));
}

bool ImageFilterChain::empty() const
{
    return m_filters.empty();
}

CvMatRaiiAdapter ImageFilterChain::apply(CvMatRaiiAdapter&& input)
{
    boost::for_each(m_filters, [&](const auto& filter) {
        input = filter->apply(std::move(input));
    });
    return std::move(input);
}

} // namespace vehlwn
