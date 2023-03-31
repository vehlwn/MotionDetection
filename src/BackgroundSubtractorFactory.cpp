#include "BackgroundSubtractorFactory.hpp"

#include "fmt/core.h"
#include "opencv2/video/background_segm.hpp"

#include <cstdlib>

namespace vehlwn {
namespace {
class OpencvBackgroundSubtractorAdapter : public IBackgroundSubtractor {
public:
    OpencvBackgroundSubtractorAdapter(cv::Ptr<cv::BackgroundSubtractor>&& impl)
        : m_impl(std::move(impl))
    {}
    virtual CvMatRaiiAdapter apply(CvMatRaiiAdapter&& image) override
    {
        CvMatRaiiAdapter fgmask;
        m_impl->apply(image.get(), fgmask.get());
        return fgmask;
    }

private:
    cv::Ptr<cv::BackgroundSubtractor> m_impl;
};
} // namespace

BackgroundSubtractorFactory::BackgroundSubtractorFactory(
    const ApplicationSettings::BackgroundSubtractor& config,
    Poco::Logger& logger)
    : m_config{config}
    , m_logger{logger}
{}

std::shared_ptr<IBackgroundSubtractor> BackgroundSubtractorFactory::create()
{
    if(const auto* const knn
       = std::get_if<vehlwn::ApplicationSettings::BackgroundSubtractor::Knn>(
           &m_config.algorithm)) {
        return std::make_shared<OpencvBackgroundSubtractorAdapter>(
            cv::createBackgroundSubtractorKNN(
                knn->history,
                knn->dist_2_threshold,
                knn->detect_shadows));
    } else if(
        const auto* const mog2
        = std::get_if<vehlwn::ApplicationSettings::BackgroundSubtractor::Mog2>(
            &m_config.algorithm)) {
        return std::make_shared<OpencvBackgroundSubtractorAdapter>(
            cv::createBackgroundSubtractorMOG2(
                mog2->history,
                mog2->var_threshold,
                mog2->detect_shadows));
    }
    poco_fatal(m_logger, "Unreachable!");
    std::exit(1);
}

} // namespace vehlwn
