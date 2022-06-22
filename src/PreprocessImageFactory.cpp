#include "PreprocessImageFactory.hpp"

#include "GaussianBlurFilter.hpp"
#include "IdentityFilter.hpp"
#include "ImageFilterChain.hpp"
#include "MedianFilter.hpp"
#include "NormalizedBoxFilter.hpp"
#include "ResizeFilter.hpp"
#include "fmt/core.h"

#include <cstdlib>
#include <memory>
#include <string>

namespace vehlwn {
PreprocessImageFactory::PreprocessImageFactory(
    std::shared_ptr<ApplicationSettings> config,
    Poco::Logger& logger)
    : m_config{std::move(config)}
    , m_logger{logger} {
}

std::shared_ptr<IImageFilter> PreprocessImageFactory::create() {
    auto ret = std::make_shared<ImageFilterChain>();
    bool has_any_filter = false;
    if(m_config->has_preprocess_resize_factor()) {
        has_any_filter = true;
        const double scale_factor = m_config->get_preprocess_resize_factor();
        poco_information(
            m_logger,
            fmt::format("preprocess.resize.factor = {}", scale_factor));
        ret->add_filter(std::make_shared<ResizeFilter>(scale_factor));
    }
    if(m_config->has_preprocess_smoothing_name()) {
        has_any_filter = true;
        const std::string name = m_config->get_preprocess_smoothing_name();
        poco_information(
            m_logger,
            fmt::format("preprocess.smoothing.name = {}", name));
        if(name == "gaussian") {
            const int kernel_size
                = m_config->get_preproccess_smoothing_kernel_size();
            poco_information(
                m_logger,
                fmt::format("preprocess.smoothing.kernel_size = {}", kernel_size));
            const double sigma = m_config->get_preprocess_smoothing_sigma(0.0);
            poco_information(
                m_logger,
                fmt::format("preprocess.smoothing.sigma = {}", sigma));
            ret->add_filter(
                std::make_shared<GaussianBlurFilter>(kernel_size, sigma));
        } else if(name == "normalized_box") {
            const int kernel_size
                = m_config->get_preproccess_smoothing_kernel_size();
            poco_information(
                m_logger,
                fmt::format("preprocess.smoothing.kernel_size = {}", kernel_size));
            ret->add_filter(std::make_shared<NormalizedBoxFilter>(kernel_size));
        } else if(name == "median") {
            const int kernel_size
                = m_config->get_preproccess_smoothing_kernel_size();
            poco_information(
                m_logger,
                fmt::format("preprocess.smoothing.kernel_size = {}", kernel_size));
            ret->add_filter(std::make_shared<MedianFilter>(kernel_size));
        } else {
            poco_fatal(
                m_logger,
                fmt::format("Unknown smoothing filter name: {}", name));
            std::abort();
        }
    }
    if(has_any_filter)
        return ret;
    else
        return std::make_shared<IdentityFilter>();
}
} // namespace vehlwn
