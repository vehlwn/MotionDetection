#pragma once

#include <utility>

#include "opencv2/core.hpp"
#include "opencv2/core/mat.hpp"

namespace vehlwn {
class MotionData {
public:
    MotionData()
        : m_moving_area{0}
    {}

    MotionData& set_frame(cv::Mat frame)
    {
        m_frame = std::move(frame);
        return *this;
    }
    const cv::Mat& frame() const
    {
        return m_frame;
    }

    MotionData& set_fgmask(cv::Mat fgmask)
    {
        m_fgmask = std::move(fgmask);
        fgmask_changed();
        return *this;
    }
    const cv::Mat& fgmask() const
    {
        return m_fgmask;
    }

private:
    void fgmask_changed()
    {
        m_moving_area = cv::countNonZero(m_fgmask);
    }
    cv::Mat m_frame;
    cv::Mat m_fgmask;
    int m_moving_area;
};
} // namespace vehlwn
