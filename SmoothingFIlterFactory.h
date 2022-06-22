#pragma once

#include "ApplicationSettings.h"
#include "ISmoothingFilter.h"
#include "Poco/Logger.h"

#include <memory>

namespace vehlwn {
class SmoothingFIlterFactory
{
public:
    SmoothingFIlterFactory(
        std::shared_ptr<ApplicationSettings> config,
        Poco::Logger& logger);
    std::shared_ptr<ISmoothingFilter> create();

private:
    std::shared_ptr<ApplicationSettings> m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
