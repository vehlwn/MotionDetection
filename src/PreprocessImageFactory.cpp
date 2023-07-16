#include "PreprocessImageFactory.hpp"

#include <cstdlib>
#include <memory>
#include <string>

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

#include "ApplicationSettings.hpp"
#include "filters/GaussianBlurFilter.hpp"
#include "filters/IdentityFilter.hpp"
#include "filters/ImageFilterChain.hpp"
#include "filters/MedianFilter.hpp"
#include "filters/NormalizedBoxFilter.hpp"
#include "filters/ResizeFilter.hpp"

namespace vehlwn {
PreprocessImageFactory::PreprocessImageFactory(
    const ApplicationSettings::Preprocess& config)
    : m_config{config}
{}

std::shared_ptr<IImageFilter> PreprocessImageFactory::create()
{
    BOOST_LOG_FUNCTION();
    auto ret = std::make_shared<ImageFilterChain>();
    bool has_any_filter = false;
    if(m_config.resize_factor) {
        has_any_filter = true;
        const double scale_factor = m_config.resize_factor.value();
        ret->add_filter(std::make_shared<ResizeFilter>(scale_factor));
    }
    if(m_config.smoothing) {
        has_any_filter = true;
        const auto& algorithm = m_config.smoothing.value().algorithm;
        using Smoothing = ApplicationSettings::Preprocess::Smoothing;
        if(const auto* normalized
           = std::get_if<Smoothing::NormalizedBox>(&algorithm)) {
            ret->add_filter(
                std::make_shared<NormalizedBoxFilter>(normalized->kernel_size));
        } else if(const auto* gaus = std::get_if<Smoothing::Gaussian>(&algorithm)) {
            ret->add_filter(std::make_shared<GaussianBlurFilter>(
                gaus->kernel_size,
                gaus->sigma));
        } else if(const auto* median = std::get_if<Smoothing::Median>(&algorithm)) {
            ret->add_filter(std::make_shared<MedianFilter>(median->kernel_size));
        } else {
            BOOST_LOG_TRIVIAL(fatal) << "Unreachable!";
            std::exit(1);
        }
    }
    if(has_any_filter) {
        return ret;
    }
    return std::make_shared<IdentityFilter>();
}
} // namespace vehlwn
