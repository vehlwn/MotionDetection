#pragma once

#include <memory>

#include "ApplicationSettings.hpp"
#include "filters/IImageFilter.hpp"

namespace vehlwn {
class PreprocessImageFactory {
public:
    PreprocessImageFactory(const ApplicationSettings::Preprocess& config);
    std::shared_ptr<IImageFilter> create();

private:
    const ApplicationSettings::Preprocess& m_config;
};
} // namespace vehlwn
