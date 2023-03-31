#pragma once

#include <opencv2/core/mat.hpp>

#include "CvMatRaiiAdapter.hpp"

namespace vehlwn {
class MotionData {
public:
    MotionData();

    MotionData& set_frame(CvMatRaiiAdapter&& frame);
    const CvMatRaiiAdapter& frame() const;

    MotionData& set_fgmask(CvMatRaiiAdapter&& fgmask);
    const CvMatRaiiAdapter& fgmask() const;

private:
    void fgmask_changed();

    CvMatRaiiAdapter m_frame;
    CvMatRaiiAdapter m_fgmask;
    int m_moving_area;
};
} // namespace vehlwn
