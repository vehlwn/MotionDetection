#include "ApplicationSettings.hpp"

#include <algorithm>
#include <cstdlib>

#include <Poco/Exception.h>
#include <Poco/NumberParser.h>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>
#include <exception>

namespace {
class ConfigParser {
public:
    explicit ConfigParser(const Poco::Util::AbstractConfiguration& config)
        : m_config{config}
    {}
    [[nodiscard]] Poco::Net::SocketAddress parse_host_and_port() const
    {
        BOOST_LOG_FUNCTION();
        std::string tmp_value;
        if(m_config.hasProperty("http_server.host_and_port")) {
            tmp_value = m_config.getString("http_server.host_and_port");
        } else {
            BOOST_LOG_TRIVIAL(fatal) << "http_server.host_and_port key not found";
            std::exit(1);
        }
        try {
            Poco::Net::SocketAddress ret{tmp_value};
            return ret;
        } catch(const Poco::Exception& ex) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Cannot create socket address: " << ex.what();
            std::exit(1);
        }
    }
    [[nodiscard]] vehlwn::ApplicationSettings::VideoCapture
        parse_video_capture() const
    {
        BOOST_LOG_FUNCTION();
        std::string filename;
        if(m_config.hasProperty("video_capture.filename")) {
            filename = m_config.getString("video_capture.filename");
        } else {
            BOOST_LOG_TRIVIAL(fatal) << "video_capture.filename key not found";
            std::exit(1);
        }
        std::optional<std::string> file_format;
        if(m_config.hasProperty("video_capture.file_format")) {
            file_format = m_config.getString("video_capture.file_format");
        }
        std::optional<std::string> video_size;
        if(m_config.hasProperty("video_capture.video_size")) {
            video_size = m_config.getString("video_capture.video_size");
        }
        std::optional<std::string> framerate;
        if(m_config.hasProperty("video_capture.framerate")) {
            framerate = m_config.getString("video_capture.framerate");
        }
        std::optional<std::string> input_format;
        if(m_config.hasProperty("video_capture.input_format")) {
            input_format = m_config.getString("video_capture.input_format");
        }
        return {
            std::move(filename),
            std::move(file_format),
            std::move(video_size),
            std::move(framerate),
            std::move(input_format)};
    }
    [[nodiscard]] vehlwn::ApplicationSettings::OutputFiles parse_output_files() const
    {
        std::string prefix;
        if(m_config.hasProperty("output_files.prefix")) {
            prefix = m_config.getString("output_files.prefix");
        }
        std::string extension;
        if(m_config.hasProperty("output_files.extension")) {
            extension = m_config.getString("output_files.extension");
        }
        std::optional<std::string> video_bitrate;
        if(m_config.hasProperty("output_files.video_bitrate")) {
            video_bitrate = m_config.getString("output_files.video_bitrate");
        }
        std::optional<std::string> audio_bitrate;
        if(m_config.hasProperty("output_files.audio_bitrate")) {
            audio_bitrate = m_config.getString("output_files.audio_bitrate");
        }
        return {
            std::move(prefix),
            std::move(extension),
            std::move(video_bitrate),
            std::move(audio_bitrate)};
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Logging parse_logging() const
    {
        std::string log_level = m_config.getString("logging.log_level", "info");
        return {std::move(log_level)};
    }
    [[nodiscard]] vehlwn::ApplicationSettings::BackgroundSubtractor::Knn
        parse_knn() const
    {
        BOOST_LOG_FUNCTION();
        vehlwn::ApplicationSettings::BackgroundSubtractor::Knn ret{};
        try {
            ret.history = m_config.getInt("background_subtractor.history", 500);
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid number for background_subtractor.history";
            std::exit(1);
        }
        if(ret.history <= 0) {
            BOOST_LOG_TRIVIAL(fatal)
                << "background_subtractor.history must be positive int";
            std::exit(1);
        }
        try {
            ret.dist_2_threshold = m_config.getDouble(
                "background_subtractor.dist_2_threshold",
                400.0);
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid number for background_subtractor.dist_2_threshold";
            std::exit(1);
        }
        if(ret.dist_2_threshold <= 0) {
            BOOST_LOG_TRIVIAL(fatal)
                << "background_subtractor.dist_2_threshold must be positive double";
            std::exit(1);
        }
        try {
            ret.detect_shadows
                = m_config.getBool("background_subtractor.detect_shadows", true);
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid bool for background_subtractor.detect_shadows";
            std::exit(1);
        }
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::BackgroundSubtractor::Mog2
        parse_mog2() const
    {
        BOOST_LOG_FUNCTION();
        vehlwn::ApplicationSettings::BackgroundSubtractor::Mog2 ret{};
        try {
            ret.history = m_config.getInt("background_subtractor.history", 500);
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid number for background_subtractor.history";
            std::exit(1);
        }
        if(ret.history <= 0) {
            BOOST_LOG_TRIVIAL(fatal)
                << "background_subtractor.history must be positive int";
            std::exit(1);
        }
        try {
            ret.var_threshold
                = m_config.getDouble("background_subtractor.var_threshold", 16.0);
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid number for background_subtractor.var_threshold";
            std::exit(1);
        }
        if(ret.var_threshold <= 0.0) {
            BOOST_LOG_TRIVIAL(fatal)
                << "background_subtractor.var_threshold must be positive double";
            std::exit(1);
        }
        try {
            ret.detect_shadows
                = m_config.getBool("background_subtractor.detect_shadows", true);
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid bool for background_subtractor.detect_shadows";
            std::exit(1);
        }
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::BackgroundSubtractor
        parse_background_subtractor() const
    {
        BOOST_LOG_FUNCTION();
        vehlwn::ApplicationSettings::BackgroundSubtractor ret;
        std::string algorithm_name;
        if(m_config.hasProperty("background_subtractor.algorithm")) {
            algorithm_name = m_config.getString("background_subtractor.algorithm");
        } else {
            BOOST_LOG_TRIVIAL(fatal)
                << "background_subtractor.algorithm key not found";
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
            BOOST_LOG_TRIVIAL(fatal) << "Unknown background_subtractor algorithm: '"
                                     << algorithm_name << "'";
            std::exit(1);
        }
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess::NormalizedBox
        parse_normalized_filter() const
    {
        BOOST_LOG_FUNCTION();
        vehlwn::ApplicationSettings::Preprocess::NormalizedBox ret{};
        try {
            ret.kernel_size = m_config.getInt("preprocess.smoothing.kernel_size");
            if(ret.kernel_size <= 0) {
                BOOST_LOG_TRIVIAL(fatal) << "kernel_size must be positive";
                std::exit(1);
            }
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid number format for normalized kernel_size";
            std::exit(1);
        } catch(const Poco::NotFoundException&) {
            BOOST_LOG_TRIVIAL(fatal) << "Normalized kernel_size key not found";
            std::exit(1);
        }
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess::Gaussian
        parse_gaussian_filter() const
    {
        BOOST_LOG_FUNCTION();
        vehlwn::ApplicationSettings::Preprocess::Gaussian ret{};
        try {
            ret.kernel_size = m_config.getInt("preprocess.smoothing.kernel_size");
            if(ret.kernel_size <= 0 || ret.kernel_size % 2 == 0) {
                BOOST_LOG_TRIVIAL(fatal)
                    << "Gaussian kernel_size must be positive and odd";
                std::exit(1);
            }
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid number format for preprocess.smoothing.kernel_size";
            std::exit(1);
        } catch(const Poco::NotFoundException&) {
            BOOST_LOG_TRIVIAL(fatal) << "Gaussian kernel_size key not found";
            std::exit(1);
        }
        try {
            ret.sigma = m_config.getDouble("preprocess.smoothing.sigma", 0.0);
            if(ret.sigma < 0.0) {
                BOOST_LOG_TRIVIAL(fatal)
                    << "Gaussian kernel standard deviation must be positive double";
                std::exit(1);
            }
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal) << "Invalid number format for Gaussian sigma";
            std::exit(1);
        }
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess::Median
        parse_median_filter() const
    {
        BOOST_LOG_FUNCTION();
        vehlwn::ApplicationSettings::Preprocess::Median ret{};
        try {
            ret.kernel_size = m_config.getInt("preprocess.smoothing.kernel_size");
            if(ret.kernel_size <= 2 || ret.kernel_size % 2 == 0) {
                BOOST_LOG_TRIVIAL(fatal)
                    << "Median kernel_size must be odd and greater than 1";
                std::exit(1);
            }
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid number format for median kernel_size";
            std::exit(1);
        } catch(const Poco::NotFoundException&) {
            BOOST_LOG_TRIVIAL(fatal) << "Median kernel_size key not found";
            std::exit(1);
        }
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess parse_preprocess() const
    {
        BOOST_LOG_FUNCTION();
        vehlwn::ApplicationSettings::Preprocess ret;
        try {
            ret.resize_factor = m_config.getDouble("preprocess.resize.factor");
            if(*ret.resize_factor <= 0.0) {
                BOOST_LOG_TRIVIAL(fatal)
                    << "preprocess.resize.factor must be positive double";
                std::exit(1);
            }
        } catch(const Poco::NotFoundException&) {
            ret.resize_factor = std::nullopt;
        } catch(const Poco::SyntaxException&) {
            BOOST_LOG_TRIVIAL(fatal)
                << "Invalid number format for preprocess.resize.factor";
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
            BOOST_LOG_TRIVIAL(fatal)
                << "Unknown smoothing filter algorithm: " << algorithm_name;
            std::exit(1);
        }
        return ret;
    }

private:
    const Poco::Util::AbstractConfiguration& m_config;
};

} // namespace

namespace vehlwn {
ApplicationSettings
    read_settings(const Poco::Util::AbstractConfiguration& config) noexcept
try {
    BOOST_LOG_FUNCTION();
    const ConfigParser p{config};
    auto host_and_port = p.parse_host_and_port();
    auto video_capture = p.parse_video_capture();
    auto output_files = p.parse_output_files();
    auto logging = p.parse_logging();
    auto background_subtractor = p.parse_background_subtractor();
    auto preprocess = p.parse_preprocess();
    return {
        host_and_port,
        std::move(video_capture),
        std::move(output_files),
        std::move(logging),
        background_subtractor,
        preprocess};
} catch(const std::exception& ex) {
    BOOST_LOG_TRIVIAL(fatal) << "Unexpected error in read_settings: " << ex.what();
    std::exit(1);
}

} // namespace vehlwn
