#include "MotionData.hpp"

#include <utility>

#include "opencv2/core.hpp"

namespace vehlwn {
MotionData::MotionData()
    : m_moving_area{0}
{}

MotionData& MotionData::set_frame(CvMatRaiiAdapter&& frame)
{
    m_frame = std::move(frame);
    return *this;
}
const CvMatRaiiAdapter& MotionData::frame() const
{
    return m_frame;
}

MotionData& MotionData::set_fgmask(CvMatRaiiAdapter&& fgmask)
{
    m_fgmask = std::move(fgmask);
    fgmask_changed();
    return *this;
}
const CvMatRaiiAdapter& MotionData::fgmask() const
{
    return m_fgmask;
}

void MotionData::fgmask_changed()
{
    m_moving_area = cv::countNonZero(m_fgmask.get());
}
} // namespace vehlwn
