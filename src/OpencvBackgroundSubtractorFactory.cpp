#include "OpencvBackgroundSubtractorFactory.hpp"

#include "fmt/core.h"

#include <cstdlib>

namespace vehlwn {
OpencvBackgroundSubtractorFactory::OpencvBackgroundSubtractorFactory(
    std::shared_ptr<ApplicationSettings> config,
    Poco::Logger& logger)
    : m_config{std::move(config)}
    , m_logger{logger}
{
}

std::shared_ptr<cv::BackgroundSubtractor> OpencvBackgroundSubtractorFactory::create()
{
    const std::string algorithm = m_config->get_background_subtractor_algorithm();
    poco_information(m_logger, "background_subtractor.algorithm = " + algorithm);
    // https://docs.opencv.org/4.5.3/de/de1/group__video__motion.html
    if(algorithm == "KNN")
    {
        const int history = m_config->get_background_subtractor_history(500);
        poco_information(
            m_logger,
            fmt::format("background_subtractor.history = {}", history));
        const double dist_2_threshold =
            m_config->get_background_subtractor_dist_2_threshold(400.0);
        poco_information(
            m_logger,
            fmt::format(
                "background_subtractor.dist_2_threshold = {}",
                dist_2_threshold));
        const bool detect_shadows =
            m_config->get_background_subtractor_detect_shadows(true);
        poco_information(
            m_logger,
            fmt::format(
                "background_subtractor.detect_shadows = {}",
                detect_shadows));
        return cv::createBackgroundSubtractorKNN(
            history,
            dist_2_threshold,
            detect_shadows);
    }
    else if(algorithm == "MOG2")
    {
        const int history = m_config->get_background_subtractor_history(500);
        poco_information(
            m_logger,
            fmt::format("background_subtractor.history = {}", history));
        const double var_threshold =
            m_config->get_background_subtractor_var_threshold(16.0);
        poco_information(
            m_logger,
            fmt::format("background_subtractor.var_threshold = {}", var_threshold));
        const bool detect_shadows =
            m_config->get_background_subtractor_detect_shadows(true);
        poco_information(
            m_logger,
            fmt::format(
                "background_subtractor.detect_shadows = {}",
                detect_shadows));
        return cv::createBackgroundSubtractorMOG2(
            history,
            var_threshold,
            detect_shadows);
    }
    // The rest algorithms are in opencv_contrib module which does not present in
    // system packages. https://docs.opencv.org/4.5.3/d2/d55/group__bgsegm.html
    poco_fatal(
        m_logger,
        fmt::format("Unknown background_subtractor algorithm: {}", algorithm));
    std::abort();
}

} // namespace vehlwn
