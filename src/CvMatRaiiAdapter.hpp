#pragma once

#include <opencv2/core/mat.hpp>

namespace vehlwn {
class CvMatRaiiAdapter {
    cv::Mat m_internal;

public:
    CvMatRaiiAdapter()
    {}
    CvMatRaiiAdapter(const cv::Mat& rhs)
        : m_internal(rhs.clone())
    {}
    CvMatRaiiAdapter(cv::Mat&& rhs) noexcept
        : m_internal(std::move(rhs))
    {}
};
} // namespace vehlwn
