#pragma once

#include "ApplicationSettings.hpp"
#include "IImageFilter.hpp"
#include "Poco/Logger.h"

#include <memory>

namespace vehlwn {
class PreprocessImageFactory {
public:
    PreprocessImageFactory(
        std::shared_ptr<ApplicationSettings> config,
        Poco::Logger& logger);
    std::shared_ptr<IImageFilter> create();

private:
    std::shared_ptr<ApplicationSettings> m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
