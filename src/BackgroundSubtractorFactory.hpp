#pragma once

#include <memory>

#include "ApplicationSettings.hpp"
#include "IBackgroundSubtractor.hpp"

namespace vehlwn {
class BackgroundSubtractorFactory {
public:
    explicit BackgroundSubtractorFactory(
        const ApplicationSettings::Segmentation::BackgroundSubtractor& config);
    std::shared_ptr<IBackgroundSubtractor> create();

private:
    const ApplicationSettings::Segmentation::BackgroundSubtractor& m_config;
};
} // namespace vehlwn
