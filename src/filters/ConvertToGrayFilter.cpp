#include "ConvertToGrayFilter.hpp"

#include <opencv2/imgproc.hpp>

namespace vehlwn {
CvMatRaiiAdapter ConvertToGrayFilter::apply(CvMatRaiiAdapter&& input)
{
    cv::cvtColor(input.get(), input.get(), cv::COLOR_BGR2GRAY);
    return std::move(input);
}
} // namespace vehlwn
