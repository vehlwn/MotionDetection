#include "OpencvBackgroundSubtractorFactory.h"

#include "fmt/core.h"

#include <cstdlib>

namespace vehlwn {
OpencvBackgroundSubtractorFactory::OpencvBackgroundSubtractorFactory(
    const Poco::Util::AbstractConfiguration& config,
    Poco::Logger& logger)
    : m_config{config}
    , m_logger{logger}
{
}

std::shared_ptr<cv::BackgroundSubtractor> OpencvBackgroundSubtractorFactory::create()
{
    const std::string algorithm =
        m_config.getString("background_subtractor.algorithm");
    poco_information(m_logger, "background_subtractor.algorithm = " + algorithm);
    // https://docs.opencv.org/4.5.3/de/de1/group__video__motion.html
    if(algorithm == "KNN")
    {
        const int history = m_config.getInt("background_subtractor.history", 500);
        poco_information(
            m_logger,
            fmt::format("background_subtractor.history = {}", history));
        const double dist2Threshold =
            m_config.getDouble("background_subtractor.dist2Threshold", 400.0);
        poco_information(
            m_logger,
            fmt::format(
                "background_subtractor.dist2Threshold = {}",
                dist2Threshold));
        const bool detectShadows =
            m_config.getBool("background_subtractor.detectShadows", true);
        poco_information(
            m_logger,
            fmt::format("background_subtractor.detectShadows = {}", detectShadows));
        return cv::createBackgroundSubtractorKNN(
            history,
            dist2Threshold,
            detectShadows);
    }
    else if(algorithm == "MOG2")
    {
        const int history = m_config.getInt("background_subtractor.history", 500);
        poco_information(
            m_logger,
            fmt::format("background_subtractor.history = {}", history));
        const double varThreshold =
            m_config.getDouble("background_subtractor.varThreshold", 16.0);
        poco_information(
            m_logger,
            fmt::format("background_subtractor.varThreshold = {}", varThreshold));
        const bool detectShadows =
            m_config.getBool("background_subtractor.detectShadows", true);
        poco_information(
            m_logger,
            fmt::format("background_subtractor.detectShadows = {}", detectShadows));
        return cv::createBackgroundSubtractorMOG2(
            history,
            varThreshold,
            detectShadows);
    }
    // The rest algorithms are in opencv_contrib module which does not present
    // in system packages.
    // https://docs.opencv.org/4.5.3/d2/d55/group__bgsegm.html
    poco_fatal(
        m_logger,
        fmt::format("Unknown background_subtractor algorithm: {}", algorithm));
    std::abort();
}

} // namespace vehlwn
