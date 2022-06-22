#pragma once

#include <opencv2/core/mat.hpp>

class MotionData {
public:
    MotionData(cv::Mat frame, cv::Mat fgmask)
        : m_frame{std::move(frame)}
        , m_fgmask{std::move(fgmask)} {
    }

    cv::Mat frame() const {
        return m_frame.clone();
    }

    cv::Mat fgmask() const {
        return m_fgmask.clone();
    }

private:
    cv::Mat m_frame;
    cv::Mat m_fgmask;
};
