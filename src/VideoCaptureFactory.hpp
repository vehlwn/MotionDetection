#pragma once

#include "ApplicationSettings.hpp"
#include "IVideoCapture.hpp"
#include "Poco/Logger.h"

#include <memory>

namespace vehlwn {
class VideoCaptureFactory {
public:
    VideoCaptureFactory(
        const ApplicationSettings::VideoCapture& config,
        Poco::Logger& logger);
    std::shared_ptr<IVideoCapture> create();

private:
    const ApplicationSettings::VideoCapture& m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
