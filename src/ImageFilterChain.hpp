#pragma once

#include "IImageFilter.hpp"

#include <memory>
#include <vector>

namespace vehlwn {
class ImageFilterChain : public IImageFilter {
public:
    void add_filter(std::shared_ptr<IImageFilter> filter);
    virtual cv::Mat apply(const cv::Mat& input) override;

private:
    std::vector<std::shared_ptr<IImageFilter>> m_filters;
};
} // namespace vehlwn
