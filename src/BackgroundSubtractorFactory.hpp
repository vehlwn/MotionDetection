#pragma once

#include <memory>

#include "ApplicationSettings.hpp"
#include "IBackgroundSubtractor.hpp"

namespace vehlwn {
class BackgroundSubtractorFactory {
public:
    BackgroundSubtractorFactory(
        const ApplicationSettings::BackgroundSubtractor& config);
    std::shared_ptr<IBackgroundSubtractor> create();

private:
    const ApplicationSettings::BackgroundSubtractor& m_config;
};
} // namespace vehlwn
