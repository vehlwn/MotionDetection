#include "ApplicationSettings.hpp"

#include <algorithm>
#include <cstdlib>

#include "Poco/Exception.h"
#include "Poco/NumberParser.h"

namespace {
class ConfigParser {
public:
    ConfigParser(
        const Poco::Util::AbstractConfiguration& config,
        Poco::Logger& logger)
        : m_config{config}
        , m_logger{logger}
    {}
    Poco::Net::SocketAddress parse_host_and_port() const
    {
        std::string tmp_value;
        try {
            tmp_value = m_config.getString("http_server.host_and_port");
        } catch(const Poco::NotFoundException&) {
            poco_fatal(m_logger, "http_server.host_and_port key not found");
            std::exit(1);
        }
        try {
            Poco::Net::SocketAddress ret{tmp_value};
            return ret;
        } catch(const Poco::Exception& ex) {
            poco_fatal(
                m_logger,
                std::string{"Cannot create socket address: "} + ex.what());
            std::exit(1);
        }
    }
    vehlwn::ApplicationSettings::VideoCapture::ApiPreference
        parse_api_preference() const
    {
        using ApiPreference
            = vehlwn::ApplicationSettings::VideoCapture::ApiPreference;
        ApiPreference ret;
        try {
            const std::string tmp
                = m_config.getString("video_capture.api_preference");
            if(tmp == "CAP_FFMPEG")
                ret = ApiPreference::CAP_FFMPEG;
            else if(tmp == "CAP_V4L2")
                ret = ApiPreference::CAP_V4L2;
            else if(tmp == "CAP_ANY")
                ret = ApiPreference::CAP_ANY;
            else {
                poco_fatal(m_logger, "Unknown api_preference");
                std::exit(1);
            }
        } catch(const Poco::NotFoundException&) {
            ret = ApiPreference::CAP_ANY;
        }
        return ret;
    }
    std::optional<vehlwn::ApplicationSettings::VideoCapture::FourccType>
        parse_fourcc() const
    {
        using FourccType = vehlwn::ApplicationSettings::VideoCapture::FourccType;
        std::optional<FourccType> ret;
        try {
            const std::string tmp = m_config.getString("video_capture.fourcc");
            if(tmp.size() == 4) {
                ret = FourccType{};
                std::copy(tmp.begin(), tmp.end(), ret->begin());
            } else {
                poco_fatal(m_logger, "Invalid fourcc format");
                std::exit(1);
            }
        } catch(const Poco::NotFoundException&) {
            ret = std::nullopt;
        }
        return ret;
    }
    std::optional<vehlwn::ApplicationSettings::VideoCapture::Size>
        parse_video_size() const
    {
        using Size = vehlwn::ApplicationSettings::VideoCapture::Size;
        std::string tmp;
        try {
            tmp = m_config.getString("video_capture.size");
        } catch(const Poco::NotFoundException&) {
            return std::nullopt;
        }
        const auto x_pos = tmp.find('x');
        if(x_pos == 0 || x_pos == tmp.size() || x_pos == std::string::npos) {
            poco_fatal(m_logger, "Invalid fourcc format");
            std::exit(1);
        }
        const std::string width_s{
            tmp.begin(),
            tmp.begin() + static_cast<std::ptrdiff_t>(x_pos)};
        const std::string height_s{
            tmp.begin() + static_cast<std::ptrdiff_t>(x_pos + 1),
            tmp.end()};
        int width{};
        if(!Poco::NumberParser::tryParse(width_s, width)) {
            poco_fatal(m_logger, "Invalid number format for width");
            std::exit(1);
        }
        if(width <= 0) {
            poco_fatal(m_logger, "Width must be positive integer");
            std::exit(1);
        }
        int height{};
        if(!Poco::NumberParser::tryParse(height_s, height)) {
            poco_fatal(m_logger, "Invalid number for height");
            std::exit(1);
        }
        if(height <= 0) {
            poco_fatal(m_logger, "height must be positive integer");
            std::exit(1);
        }
        return Size{width, height};
    }
    std::optional<double> parse_framerate() const
    try {
        const double ret = m_config.getDouble("video_capture.framerate");
        if(ret <= 0.0) {
            poco_fatal(m_logger, "Invalid valid for video_capture.framerate");
            std::exit(1);
        }
        return std::optional{ret};
    } catch(const Poco::NotFoundException&) {
        return std::nullopt;
    } catch(const Poco::SyntaxException&) {
        poco_fatal(m_logger, "Invalid number format for video_capture.framerate");
        std::exit(1);
    }
    vehlwn::ApplicationSettings::VideoCapture parse_video_capture() const
    {
        std::string filename;
        try {
            filename = m_config.getString("video_capture.filename");
        } catch(const Poco::NotFoundException&) {
            poco_fatal(m_logger, "video_capture.filename key not found");
            std::exit(1);
        }
        auto api_preference = parse_api_preference();
        auto fourcc = parse_fourcc();
        auto size = parse_video_size();
        auto framerate = parse_framerate();
        return {std::move(filename), api_preference, fourcc, size, framerate};
    }
    vehlwn::ApplicationSettings::BackgroundSubtractor::Knn parse_knn() const
    {
        vehlwn::ApplicationSettings::BackgroundSubtractor::Knn ret;
        try {
            ret.history = m_config.getInt("background_subtractor.history", 500);
        } catch(const Poco::SyntaxException&) {
            poco_fatal(m_logger, "Invalid number for background_subtractor.history");
            std::exit(1);
        }
        if(ret.history <= 0) {
            poco_fatal(
                m_logger,
                "background_subtractor.history must be positive int");
            std::exit(1);
        }
        try {
            ret.dist_2_threshold = m_config.getDouble(
                "background_subtractor.dist_2_threshold",
                400.0);
        } catch(const Poco::SyntaxException&) {
            poco_fatal(
                m_logger,
                "Invalid number for background_subtractor.dist_2_threshold");
            std::exit(1);
        }
        if(ret.dist_2_threshold <= 0) {
            poco_fatal(
                m_logger,
                "background_subtractor.dist_2_threshold must be positive double");
            std::exit(1);
        }
        try {
            ret.detect_shadows
                = m_config.getBool("background_subtractor.detect_shadows", true);
        } catch(const Poco::SyntaxException&) {
            poco_fatal(
                m_logger,
                "Invalid bool for background_subtractor.detect_shadows");
            std::exit(1);
        }
        return ret;
    }
    vehlwn::ApplicationSettings::BackgroundSubtractor::Mog2 parse_mog2() const
    {
        vehlwn::ApplicationSettings::BackgroundSubtractor::Mog2 ret;
        try {
            ret.history = m_config.getInt("background_subtractor.history", 500);
        } catch(const Poco::SyntaxException&) {
            poco_fatal(m_logger, "Invalid number for background_subtractor.history");
            std::exit(1);
        }
        if(ret.history <= 0) {
            poco_fatal(
                m_logger,
                "background_subtractor.history must be positive int");
            std::exit(1);
        }
        try {
            ret.var_threshold
                = m_config.getDouble("background_subtractor.var_threshold", 16.0);
        } catch(const Poco::SyntaxException&) {
            poco_fatal(
                m_logger,
                "Invalid number for background_subtractor.var_threshold");
            std::exit(1);
        }
        if(ret.var_threshold <= 0.0) {
            poco_fatal(
                m_logger,
                "background_subtractor.var_threshold must be positive double");
            std::exit(1);
        }
        try {
            ret.detect_shadows
                = m_config.getBool("background_subtractor.detect_shadows", true);
        } catch(const Poco::SyntaxException&) {
            poco_fatal(
                m_logger,
                "Invalid bool for background_subtractor.detect_shadows");
            std::exit(1);
        }
        return ret;
    }
    vehlwn::ApplicationSettings::BackgroundSubtractor
        parse_background_subtractor() const
    {
        vehlwn::ApplicationSettings::BackgroundSubtractor ret;
        std::string algorithm_name;
        try {
            algorithm_name = m_config.getString("background_subtractor.algorithm");
        } catch(const Poco::NotFoundException&) {
            poco_fatal(m_logger, "background_subtractor.algorithm key not found");
            std::exit(1);
        }
        // https://docs.opencv.org/4.5.3/de/de1/group__video__motion.html
        if(algorithm_name == "KNN") {
            ret.algorithm = parse_knn();
        } else if(algorithm_name == "MOG2") {
            ret.algorithm = parse_mog2();
        } else {
            // The rest algorithms are in opencv_contrib module which does not
            // present in system packages.
            // https://docs.opencv.org/4.5.3/d2/d55/group__bgsegm.html
            poco_fatal(
                m_logger,
                "Unknown background_subtractor algorithm: '" + algorithm_name + "'");
            std::exit(1);
        }
        return ret;
    }
    vehlwn::ApplicationSettings::Preprocess::NormalizedBox
        parse_normalized_filter() const
    {
        vehlwn::ApplicationSettings::Preprocess::NormalizedBox ret;
        try {
            ret.kernel_size = m_config.getInt("preprocess.smoothing.kernel_size");
            if(ret.kernel_size <= 0) {
                poco_fatal(m_logger, "kernel_size must be positive");
                std::exit(1);
            }
        } catch(const Poco::SyntaxException&) {
            poco_fatal(m_logger, "Invalid number format for normalized kernel_size");
            std::exit(1);
        } catch(const Poco::NotFoundException&) {
            poco_fatal(m_logger, "Normalized kernel_size key not found");
            std::exit(1);
        }
        return ret;
    }
    vehlwn::ApplicationSettings::Preprocess::Gaussian parse_gaussian_filter() const
    {
        vehlwn::ApplicationSettings::Preprocess::Gaussian ret;
        try {
            ret.kernel_size = m_config.getInt("preprocess.smoothing.kernel_size");
            if(ret.kernel_size <= 0 || ret.kernel_size % 2 == 0) {
                poco_fatal(
                    m_logger,
                    "Gaussian kernel_size must be positive and odd");
                std::exit(1);
            }
        } catch(const Poco::SyntaxException&) {
            poco_fatal(
                m_logger,
                "Invalid number format for preprocess.smoothing.kernel_size");
            std::exit(1);
        } catch(const Poco::NotFoundException&) {
            poco_fatal(m_logger, "Gaussian kernel_size key not found");
            std::exit(1);
        }
        try {
            ret.sigma = m_config.getDouble("preprocess.smoothing.sigma", 0.0);
            if(ret.sigma < 0.0) {
                poco_fatal(
                    m_logger,
                    "Gaussian kernel standard deviation must be positive "
                    "double");
                std::exit(1);
            }
        } catch(const Poco::SyntaxException&) {
            poco_fatal(m_logger, "Invalid number format for Gaussian sigma");
            std::exit(1);
        }
        return ret;
    }
    vehlwn::ApplicationSettings::Preprocess::Median parse_median_filter() const
    {
        vehlwn::ApplicationSettings::Preprocess::Median ret;
        try {
            ret.kernel_size = m_config.getInt("preprocess.smoothing.kernel_size");
            if(ret.kernel_size <= 2 || ret.kernel_size % 2 == 0) {
                poco_fatal(
                    m_logger,
                    "Median kernel_size must be odd and greater than 1");
                std::exit(1);
            }
        } catch(const Poco::SyntaxException&) {
            poco_fatal(m_logger, "Invalid number format for median kernel_size");
            std::exit(1);
        } catch(const Poco::NotFoundException&) {
            poco_fatal(m_logger, "Median kernel_size key not found");
            std::exit(1);
        }
        return ret;
    }
    vehlwn::ApplicationSettings::Preprocess parse_preprocess() const
    {
        vehlwn::ApplicationSettings::Preprocess ret;
        try {
            ret.resize_factor = m_config.getDouble("preprocess.resize.factor");
            if(*ret.resize_factor <= 0.0) {
                poco_fatal(
                    m_logger,
                    "preprocess.resize.factor must be positive double");
                std::exit(1);
            }
        } catch(const Poco::NotFoundException&) {
            ret.resize_factor = std::nullopt;
        } catch(const Poco::SyntaxException&) {
            poco_fatal(
                m_logger,
                "Invalid number format for preprocess.resize.factor");
            std::exit(1);
        }
        std::string algorithm_name;
        try {
            algorithm_name = m_config.getString("preprocess.smoothing.algorithm");
        } catch(const Poco::NotFoundException&) {
            ret.smoothing = std::nullopt;
            return ret;
        }
        if(algorithm_name == "normalized_box") {
            ret.smoothing = parse_normalized_filter();
        } else if(algorithm_name == "gaussian") {
            ret.smoothing = parse_gaussian_filter();
        } else if(algorithm_name == "median") {
            ret.smoothing = parse_median_filter();
        } else {
            poco_fatal(
                m_logger,
                "Unknown smoothing filter algorithm: " + algorithm_name);
            std::exit(1);
        }
        return ret;
    }

private:
    const Poco::Util::AbstractConfiguration& m_config;
    Poco::Logger& m_logger;
};

} // namespace

namespace vehlwn {
ApplicationSettings read_settings(
    const Poco::Util::AbstractConfiguration& config,
    Poco::Logger& logger) noexcept
{
    ConfigParser p{config, logger};
    auto host_and_port = p.parse_host_and_port();
    auto video_capture = p.parse_video_capture();
    auto background_subtractor = p.parse_background_subtractor();
    auto preprocess = p.parse_preprocess();
    return {
        host_and_port,
        std::move(video_capture),
        background_subtractor,
        preprocess};
}

} // namespace vehlwn
