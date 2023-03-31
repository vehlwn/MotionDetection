#pragma once

#include <opencv2/core/mat.hpp>

namespace vehlwn {
class CvMatRaiiAdapter {
    cv::Mat m_internal;

public:
    CvMatRaiiAdapter()
    {}
    CvMatRaiiAdapter(const CvMatRaiiAdapter&) = delete;
    CvMatRaiiAdapter(CvMatRaiiAdapter&&) = default;
    CvMatRaiiAdapter& operator=(const CvMatRaiiAdapter&) = delete;
    CvMatRaiiAdapter& operator=(CvMatRaiiAdapter&&) = default;

    explicit CvMatRaiiAdapter(cv::Mat&& rhs) noexcept
        : m_internal(std::move(rhs))
    {}
    const cv::Mat& get() const
    {
        return m_internal;
    }
    cv::Mat& get()
    {
        return m_internal;
    }
};
} // namespace vehlwn
