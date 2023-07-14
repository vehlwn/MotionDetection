#pragma once

#include <opencv2/core/mat.hpp>

#include "CvMatRaiiAdapter.hpp"

namespace vehlwn {
class MotionData {
public:
    MotionData();

    MotionData& set_frame(CvMatRaiiAdapter&& frame);
    [[nodiscard]] const CvMatRaiiAdapter& frame() const;

    MotionData& set_fgmask(CvMatRaiiAdapter&& fgmask);
    [[nodiscard]] const CvMatRaiiAdapter& fgmask() const;

    [[nodiscard]] int moving_area() const;

private:
    void fgmask_changed();

    CvMatRaiiAdapter m_frame;
    CvMatRaiiAdapter m_fgmask;
    int m_moving_area;
};
} // namespace vehlwn
