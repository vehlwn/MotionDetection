#pragma once

#include "ApplicationSettings.hpp"
#include "Poco/Logger.h"
#include "opencv2/video/background_segm.hpp"

#include <memory>

namespace vehlwn {
class OpencvBackgroundSubtractorFactory {
public:
    OpencvBackgroundSubtractorFactory(
        std::shared_ptr<ApplicationSettings> config,
        Poco::Logger& logger);
    std::shared_ptr<cv::BackgroundSubtractor> create();

private:
    std::shared_ptr<ApplicationSettings> m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
