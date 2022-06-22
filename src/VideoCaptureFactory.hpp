#pragma once

#include "ApplicationSettings.hpp"
#include "IVideoCapture.hpp"
#include "Poco/Logger.h"

#include <memory>

namespace vehlwn {
class VideoCaptureFactory
{
public:
    VideoCaptureFactory(
        std::shared_ptr<ApplicationSettings> config,
        Poco::Logger& logger);
    std::shared_ptr<IVideoCapture> create();

private:
    std::shared_ptr<ApplicationSettings> m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
