#pragma once

#include "ApplicationSettings.hpp"
#include "IBackgroundSubtractor.hpp"
#include "Poco/Logger.h"

#include <memory>

namespace vehlwn {
class BackgroundSubtractorFactory {
public:
    BackgroundSubtractorFactory(
        std::shared_ptr<ApplicationSettings> config,
        Poco::Logger& logger);
    std::shared_ptr<IBackgroundSubtractor> create();

private:
    std::shared_ptr<ApplicationSettings> m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
