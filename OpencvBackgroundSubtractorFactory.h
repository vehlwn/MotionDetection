#pragma once

#include "opencv2/video/background_segm.hpp"
#include "Poco/Logger.h"
#include "Poco/Util/AbstractConfiguration.h"


namespace vehlwn {
class OpencvBackgroundSubtractorFactory
{
public:
    OpencvBackgroundSubtractorFactory(
        const Poco::Util::AbstractConfiguration& config,
        Poco::Logger& logger);
    std::shared_ptr<cv::BackgroundSubtractor> create();

private:
    const Poco::Util::AbstractConfiguration& m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
