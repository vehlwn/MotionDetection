#include "ApplicationSettings.hpp"

#include <cstdlib>
#include <exception>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/value.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

#include "Config.hpp"
#include "ErrorWithContext.hpp"

namespace {
constexpr std::string_view CONFIG_FILE_NAME = "app.json";

vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::Knn
    parse_knn(const boost::json::object& back_subtr_obj)
{
    BOOST_LOG_FUNCTION();
    auto ret
        = vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::Knn();
    ret.history = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = back_subtr_obj.find("history");
               it != back_subtr_obj.end()) {
                const auto tmp = it->value().to_number<int>();
                if(tmp <= 0) {
                    throw std::runtime_error(
                        "background_subtractor.history must be positive int");
                }
                return tmp;
            }
            return 500;
        },
        "Failed to parse background_subtractor.history");
    ret.dist_2_threshold = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = back_subtr_obj.find("dist_2_threshold");
               it != back_subtr_obj.end()) {
                const auto tmp = it->value().to_number<double>();
                if(tmp <= 0) {
                    throw std::runtime_error(
                        "background_subtractor.dist_2_threshold must be "
                        "positive double");
                }
                return tmp;
            }
            return 400.0;
        },
        "Failed to parse background_subtractor.dist_2_threshold");
    ret.detect_shadows = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = back_subtr_obj.find("detect_shadows");
               it != back_subtr_obj.end()) {
                const auto tmp = it->value().as_bool();
                return tmp;
            }
            return false;
        },
        "Failed to parse background_subtractor.detect_shadows");
    return ret;
}

vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::Mog2
    parse_mog2(const boost::json::object& back_subtr_obj)
{
    BOOST_LOG_FUNCTION();
    auto ret
        = vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::Mog2();
    ret.history = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = back_subtr_obj.find("history");
               it != back_subtr_obj.end()) {
                const auto tmp = it->value().to_number<int>();
                if(tmp <= 0) {
                    throw std::runtime_error(
                        "background_subtractor.history must be positive int");
                }
                return tmp;
            }
            return 500;
        },
        "Failed to parse background_subtractor.history");
    ret.var_threshold = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = back_subtr_obj.find("var_threshold");
               it != back_subtr_obj.end()) {
                const auto tmp = it->value().to_number<double>();
                if(tmp <= 0) {
                    throw std::runtime_error(
                        "background_subtractor.var_threshold must be "
                        "positive double");
                }
                return tmp;
            }
            return 16.0;
        },
        "Failed to parse background_subtractor.dist_2_threshold");
    ret.detect_shadows = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = back_subtr_obj.find("detect_shadows");
               it != back_subtr_obj.end()) {
                const auto tmp = it->value().as_bool();
                return tmp;
            }
            return false;
        },
        "Failed to parse background_subtractor.detect_shadows");
    return ret;
}

vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor
    parse_background_subtractor(const boost::json::object& back_subtr_obj)
{
    BOOST_LOG_FUNCTION();
    const auto algorithm_name = vehlwn::invoke_with_error_context_str(
        [&] {
            auto tmp = back_subtr_obj.at("algorithm").as_string();
            return std::string(tmp.begin(), tmp.end());
        },
        "background_subtractor.algorithm key not found");

    auto ret = vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor();
    // https://docs.opencv.org/4.5.3/de/de1/group__video__motion.html
    if(algorithm_name == "KNN") {
        ret.algorithm = parse_knn(back_subtr_obj);
    } else if(algorithm_name == "MOG2") {
        ret.algorithm = parse_mog2(back_subtr_obj);
    } else {
        // The rest algorithms are in opencv_contrib module which does not
        // present in system packages.
        // https://docs.opencv.org/4.5.3/d2/d55/group__bgsegm.html
        throw std::runtime_error(
            "Unknown background_subtractor algorithm: '" + algorithm_name + "'");
    }
    return ret;
}

vehlwn::ApplicationSettings::Preprocess::Smoothing::NormalizedBox
    parse_normalized_filter(const boost::json::object& smooth_obj)
{
    BOOST_LOG_FUNCTION();
    auto ret = vehlwn::ApplicationSettings::Preprocess::Smoothing::NormalizedBox();
    ret.kernel_size = vehlwn::invoke_with_error_context_str(
        [&] {
            auto tmp = smooth_obj.at("kernel_size").as_int64();
            if(tmp <= 0) {
                throw std::runtime_error("kernel_size must be positive");
            }
            return static_cast<int>(tmp);
        },
        "Invalid number for normalized kernel_size");
    return ret;
}

vehlwn::ApplicationSettings::Preprocess::Smoothing::Gaussian
    parse_gaussian_filter(const boost::json::object& smooth_obj)
{
    BOOST_LOG_FUNCTION();
    auto ret = vehlwn::ApplicationSettings::Preprocess::Smoothing::Gaussian();
    ret.kernel_size = vehlwn::invoke_with_error_context_str(
        [&] {
            auto tmp = smooth_obj.at("kernel_size").as_int64();
            if(tmp <= 0 || tmp % 2 == 0) {
                throw std::runtime_error(
                    "Gaussian kernel_size must be positive and odd");
            }
            return static_cast<int>(tmp);
        },
        "Invalid number format for Gaussian kernel_size");
    ret.sigma = [&] {
        if(const auto* tmp = smooth_obj.at("sigma").if_double()) {
            if(*tmp < 0.0) {
                throw std::runtime_error("Gaussian kernel standard deviation "
                                         "must non negative double");
            }
            return *tmp;
        }
        return 0.0;
    }();
    return ret;
}

vehlwn::ApplicationSettings::Preprocess::Smoothing::Median
    parse_median_filter(const boost::json::object& smooth_obj)
{
    BOOST_LOG_FUNCTION();
    auto ret = vehlwn::ApplicationSettings::Preprocess::Smoothing::Median();
    ret.kernel_size = vehlwn::invoke_with_error_context_str(
        [&] {
            auto tmp = smooth_obj.at("kernel_size").as_int64();
            if(tmp <= 2 || tmp % 2 == 0) {
                throw std::runtime_error(
                    "Median kernel_size must be odd and greater than 1");
            }
            return static_cast<int>(tmp);
        },
        "Invalid number format for median kernel_size");
    return ret;
}

vehlwn::ApplicationSettings::Preprocess::Smoothing
    parse_smoothing(const boost::json::object& smooth_obj)
{
    const auto algorithm_name = [&] {
        auto tmp = smooth_obj.at("algorithm").as_string();
        return std::string(tmp.begin(), tmp.end());
    }();
    if(algorithm_name == "normalized_box") {
        auto algorithm = vehlwn::invoke_with_error_context_str(
            [&] { return parse_normalized_filter(smooth_obj); },
            "Failed to parse normalized_box parameters");
        return {algorithm};
    }
    if(algorithm_name == "gaussian") {
        auto algorithm = vehlwn::invoke_with_error_context_str(
            [&] { return parse_gaussian_filter(smooth_obj); },
            "Failed to parse gaussian parameters");
        return {algorithm};
    }
    if(algorithm_name == "median") {
        auto algorithm = parse_median_filter(smooth_obj);
        return {algorithm};
    }
    throw std::runtime_error("Unknown smmothing algorithm: " + algorithm_name);
}

class ConfigParser {
    boost::json::value m_config;

public:
    explicit ConfigParser(boost::json::value&& config)
        : m_config(std::move(config))
    {}

    [[nodiscard]] vehlwn::ApplicationSettings::VideoCapture
        parse_video_capture() const
    {
        const auto video_cap_obj = vehlwn::invoke_with_error_context_str(
            [&] { return m_config.at("video_capture").as_object(); },
            "video_capture key not found");
        auto filename = vehlwn::invoke_with_error_context_str(
            [&] {
                const auto tmp = video_cap_obj.at("filename").as_string();
                return std::string(tmp.begin(), tmp.end());
            },
            "video_capture.filename key not found");
        auto file_format = [&]() -> std::optional<std::string> {
            if(const auto it = video_cap_obj.find("file_format");
               it != video_cap_obj.end() && it->value().is_string()) {
                const auto tmp = it->value().as_string();
                return std::string(tmp.begin(), tmp.end());
            }
            return std::nullopt;
        }();
        auto video_size = [&]() -> std::optional<std::string> {
            if(const auto it = video_cap_obj.find("video_size");
               it != video_cap_obj.end() && it->value().is_string()) {
                const auto tmp = it->value().as_string();
                return std::string(tmp.begin(), tmp.end());
            }
            return std::nullopt;
        }();
        auto framerate = [&]() -> std::optional<std::string> {
            if(const auto it = video_cap_obj.find("framerate");
               it != video_cap_obj.end() && it->value().is_string()) {
                const auto tmp = it->value().as_string();
                return std::string(tmp.begin(), tmp.end());
            }
            return std::nullopt;
        }();
        auto input_format = [&]() -> std::optional<std::string> {
            if(const auto it = video_cap_obj.find("input_format");
               it != video_cap_obj.end() && it->value().is_string()) {
                const auto tmp = it->value().as_string();
                return std::string(tmp.begin(), tmp.end());
            }
            return std::nullopt;
        }();
        return {
            std::move(filename),
            std::move(file_format),
            std::move(video_size),
            std::move(framerate),
            std::move(input_format)};
    }

    [[nodiscard]] vehlwn::ApplicationSettings::OutputFiles parse_output_files() const
    {
        const auto out_file_obj = vehlwn::invoke_with_error_context_str(
            [&] { return m_config.at("output_files").as_object(); },
            "output_files key not found");

        auto prefix = vehlwn::invoke_with_error_context_str(
            [&] {
                const auto tmp = out_file_obj.at("prefix").as_string();
                return std::string(tmp.begin(), tmp.end());
            },
            "output_files.prefix key not found");
        auto extension = vehlwn::invoke_with_error_context_str(
            [&] {
                const auto tmp = out_file_obj.at("extension").as_string();
                if(tmp.empty()) {
                    throw std::runtime_error("extension is an empty string");
                }
                return std::string(tmp.begin(), tmp.end());
            },
            "Failed to parse output_files.extension");
        auto video_bitrate = [&]() -> std::optional<std::string> {
            if(const auto it = out_file_obj.find("video_bitrate");
               it != out_file_obj.end() && it->value().is_string()) {
                const auto tmp = it->value().as_string();
                return std::string(tmp.begin(), tmp.end());
            }
            return std::nullopt;
        }();
        auto audio_bitrate = [&]() -> std::optional<std::string> {
            if(const auto it = out_file_obj.find("audio_bitrate");
               it != out_file_obj.end() && it->value().is_string()) {
                const auto tmp = it->value().as_string();
                return std::string(tmp.begin(), tmp.end());
            }
            return std::nullopt;
        }();
        return {
            std::move(prefix),
            std::move(extension),
            std::move(video_bitrate),
            std::move(audio_bitrate)};
    }

    [[nodiscard]] vehlwn::ApplicationSettings::Logging parse_logging() const
    {
        std::string app_level = "info";
        std::string ffmpeg_level = "warning";

        if(const auto it = m_config.as_object().find("logging");
           it != m_config.as_object().end() && it->value().is_object()) {
            const auto logging_obj = it->value().as_object();

            if(const auto app_it = logging_obj.find("app_level");
               app_it != logging_obj.end() && app_it->value().is_string()) {
                const auto tmp = app_it->value().as_string();
                app_level = {tmp.begin(), tmp.end()};
            }
            if(const auto ffmpeg_it = logging_obj.find("ffmpeg_level");
               ffmpeg_it != logging_obj.end() && ffmpeg_it->value().is_string()) {
                const auto tmp = ffmpeg_it->value().as_string();
                ffmpeg_level = {tmp.begin(), tmp.end()};
            }
        }
        return {std::move(app_level), std::move(ffmpeg_level)};
    }

    [[nodiscard]] vehlwn::ApplicationSettings::Segmentation
        parse_segmentation() const
    {
        auto ret = vehlwn::ApplicationSettings::Segmentation();

        const auto segm_obj = vehlwn::invoke_with_error_context_str(
            [&] { return m_config.at("segmentation").as_object(); },
            "segmentation object not found");

        const auto back_subtr_obj = vehlwn::invoke_with_error_context_str(
            [&] { return segm_obj.at("background_subtractor").as_object(); },
            "segmentation.background_subtractor object not found");

        ret.background_subtractor = vehlwn::invoke_with_error_context_str(
            [&] { return parse_background_subtractor(back_subtr_obj); },
            "Failed to parse segmentation.background_subtractor");
        ret.min_moving_area = vehlwn::invoke_with_error_context_str(
            [&] {
                if(const auto it = segm_obj.find("min_moving_area");
                   it != segm_obj.end()) {
                    const auto tmp = it->value().to_number<int>();
                    if(tmp < 0) {
                        throw std::runtime_error(
                            "min_moving_area cannot be negative");
                    }
                    return tmp;
                }
                return 500;
            },
            "Failed to parse segmentation.min_moving_area");
        ret.delta_without_motion = vehlwn::invoke_with_error_context_str(
            [&] {
                if(const auto it = segm_obj.find("delta_without_motion");
                   it != segm_obj.end()) {
                    const auto tmp = it->value().to_number<double>();
                    if(tmp < 0) {
                        throw std::runtime_error(
                            "delta_without_motion cannot be negative");
                    }
                    return tmp;
                }
                return 5.0;
            },
            "Failed to parse segmentation.delta_without_motion");
        return ret;
    }

    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess parse_preprocess() const
    {
        auto ret = vehlwn::ApplicationSettings::Preprocess();
        if(const auto preprocess_it = m_config.as_object().find("preprocess");
           preprocess_it != m_config.as_object().end()
           && preprocess_it->value().is_object()) {
            const auto preprocess_obj = preprocess_it->value().as_object();

            ret.resize_factor = vehlwn::invoke_with_error_context_str(
                [&]() -> std::optional<double> {
                    if(const auto it = preprocess_obj.find("resize_factor");
                       it != preprocess_obj.end()) {
                        const auto tmp = it->value().to_number<double>();
                        if(tmp <= 0) {
                            throw std::runtime_error(
                                "preprocess.resize_factor must be positive double");
                        }
                        return tmp;
                    }
                    return std::nullopt;
                },
                "Failed to parse preprocess.resize_factor");

            if(const auto smoothing_it = preprocess_obj.find("smoothing");
               smoothing_it != preprocess_obj.end()
               && smoothing_it->value().is_object()) {
                const auto smooth_obj = smoothing_it->value().as_object();
                ret.smoothing = parse_smoothing(smooth_obj);
            }
        }
        return ret;
    }
};

} // namespace

namespace vehlwn {
ApplicationSettings read_settings() noexcept
try {
    BOOST_LOG_FUNCTION();
    const auto config_path
        = std::string(CONFIG_DIR) + "/" + std::string(CONFIG_FILE_NAME);
    auto is = invoke_with_error_context_fun(
        [&] { return std::ifstream(config_path, std::ios::binary); },
        [&] {
            std::ostringstream os;
            os << "Failed to open config file " << config_path;
            return os.str();
        });
    auto v = invoke_with_error_context_fun(
        [&] { return boost::json::parse(is); },
        [&] {
            std::ostringstream os;
            os << "Failed to parse config file " << config_path;
            return os.str();
        });

    const auto p = ConfigParser(std::move(v));
    auto video_capture = p.parse_video_capture();
    auto output_files = p.parse_output_files();
    auto logging = p.parse_logging();
    auto segmentation = p.parse_segmentation();
    auto preprocess = p.parse_preprocess();
    return {
        std::move(video_capture),
        std::move(output_files),
        std::move(logging),
        segmentation,
        preprocess};
} catch(const std::exception& ex) {
    BOOST_LOG_TRIVIAL(fatal) << "Unexpected error in read_settings: " << ex.what();
    std::exit(1);
}

} // namespace vehlwn
