#pragma once

#include <opencv2/core/mat.hpp>

class MotionData
{
public:
    MotionData(cv::Mat frame, cv::Mat fgmask)
        : m_frame{std::move(frame)}
        , m_fgmask{std::move(fgmask)}
    {
    }

private:
    cv::Mat m_frame;
    cv::Mat m_fgmask;
};
