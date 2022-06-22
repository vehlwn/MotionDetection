#pragma once

#include "ApplicationSettings.hpp"
#include "Poco/Logger.h"
#include "filters/IImageFilter.hpp"

#include <memory>

namespace vehlwn {
class PreprocessImageFactory {
public:
    PreprocessImageFactory(
        const ApplicationSettings::Preprocess& config,
        Poco::Logger& logger);
    std::shared_ptr<IImageFilter> create();

private:
    const ApplicationSettings::Preprocess& m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
