#pragma once

#include "ApplicationSettings.hpp"
#include "IBackgroundSubtractor.hpp"
#include "Poco/Logger.h"

#include <memory>

namespace vehlwn {
class BackgroundSubtractorFactory {
public:
    BackgroundSubtractorFactory(
        const ApplicationSettings::BackgroundSubtractor& config,
        Poco::Logger& logger);
    std::shared_ptr<IBackgroundSubtractor> create();

private:
    const ApplicationSettings::BackgroundSubtractor& m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
