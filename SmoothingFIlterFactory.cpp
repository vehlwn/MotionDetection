#include "SmoothingFIlterFactory.h"

#include "GaussianBlurFilter.h"
#include "IdentityFilter.h"
#include "MedianFilter.h"
#include "NormalizedBoxFilter.h"
#include "fmt/core.h"

#include <cstdlib>
#include <memory>
#include <string>

namespace vehlwn {
SmoothingFIlterFactory::SmoothingFIlterFactory(
    std::shared_ptr<ApplicationSettings> config,
    Poco::Logger& logger)
    : m_config{std::move(config)}
    , m_logger{logger}
{
}

std::shared_ptr<ISmoothingFilter> SmoothingFIlterFactory::create()
{
    if(!m_config->has_smoothing_filter_name())
    {
        poco_information(
            m_logger,
            "No smoothing_filter name given. Choosing IdentityFilter");
        return std::make_shared<IdentityFilter>();
    }
    const std::string name = m_config->get_smoothing_filter_name();
    poco_information(m_logger, fmt::format("smoothing_filter.name = {}", name));
    if(name == "gaussian")
    {
        const int kernel_size = m_config->get_smoothing_filter_kernel_size();
        poco_information(
            m_logger,
            fmt::format("smoothing_filter.kernel_size = {}", kernel_size));
        const double sigma = m_config->get_smoothing_filter_sigma(0.0);
        poco_information(
            m_logger,
            fmt::format("smoothing_filter.sigma = {}", sigma));
        return std::make_shared<GaussianBlurFilter>(kernel_size, sigma);
    }
    else if(name == "normalized_box")
    {
        const int kernel_size = m_config->get_smoothing_filter_kernel_size();
        poco_information(
            m_logger,
            fmt::format("smoothing_filter.kernel_size = {}", kernel_size));
        return std::make_shared<NormalizedBoxFilter>(kernel_size);
    }
    else if(name == "median")
    {
        const int kernel_size = m_config->get_smoothing_filter_kernel_size();
        poco_information(
            m_logger,
            fmt::format("smoothing_filter.kernel_size = {}", kernel_size));
        return std::make_shared<MedianFilter>(kernel_size);
    }
    poco_fatal(m_logger, fmt::format("Unknown smoothing filter name: {}", name));
    std::abort();
}
} // namespace vehlwn
