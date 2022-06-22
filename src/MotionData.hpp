#pragma once

#include "opencv2/core/mat.hpp"

namespace vehlwn {
class MotionData {
public:
    MotionData();

    MotionData& set_frame(cv::Mat frame);
    const cv::Mat& frame() const;

    MotionData& set_fgmask(cv::Mat fgmask);
    const cv::Mat& fgmask() const;

private:
    void fgmask_changed();

    cv::Mat m_frame;
    cv::Mat m_fgmask;
    int m_moving_area;
};
} // namespace vehlwn
