#pragma once

#include "IVideoCapture.h"
#include "Poco/Logger.h"
#include "Poco/Util/AbstractConfiguration.h"

namespace vehlwn {
class VideoCaptureFactory
{
public:
    VideoCaptureFactory(
        const Poco::Util::AbstractConfiguration& config,
        Poco::Logger& logger);
    std::shared_ptr<IVideoCapture> create();

private:
    const Poco::Util::AbstractConfiguration& m_config;
    Poco::Logger& m_logger;
};
} // namespace vehlwn
