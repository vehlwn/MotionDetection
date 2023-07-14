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

class ConfigParser {
    const boost::json::value& m_config;

public:
    explicit ConfigParser(const boost::json::value& config)
        : m_config(config)
    {}
    [[nodiscard]] vehlwn::ApplicationSettings::VideoCapture
        parse_video_capture() const
    {
        auto filename = vehlwn::invoke_with_error_context_str(
            [&] {
                return m_config.at_pointer("/video_capture/filename").as_string();
            },
            "video_capture.filename key not found");
        auto file_format = [&]() -> std::optional<std::string> {
            if(m_config.at("video_capture").as_object().contains("file_format")) {
                if(const auto tmp
                   = m_config.at_pointer("/video_capture/file_format").if_string()) {
                    return std::string(tmp->begin(), tmp->end());
                }
            }
            return std::nullopt;
        }();
        auto video_size = [&]() -> std::optional<std::string> {
            if(m_config.at("video_capture").as_object().contains("video_size")) {
                if(const auto tmp
                   = m_config.at_pointer("/video_capture/video_size").if_string()) {
                    return std::string(tmp->begin(), tmp->end());
                }
            }
            return std::nullopt;
        }();
        auto framerate = [&]() -> std::optional<std::string> {
            if(m_config.at("video_capture").as_object().contains("framerate")) {
                if(const auto tmp
                   = m_config.at_pointer("/video_capture/framerate").if_string()) {
                    return std::string(tmp->begin(), tmp->end());
                }
            }
            return std::nullopt;
        }();
        auto input_format = [&]() -> std::optional<std::string> {
            if(m_config.at("video_capture").as_object().contains("input_format")) {
                if(const auto tmp
                   = m_config.at_pointer("/video_capture/input_format")
                         .if_string()) {
                    return std::string(tmp->begin(), tmp->end());
                }
            }
            return std::nullopt;
        }();
        return {
            {filename.begin(), filename.end()},
            std::move(file_format),
            std::move(video_size),
            std::move(framerate),
            std::move(input_format)};
    }
    [[nodiscard]] vehlwn::ApplicationSettings::OutputFiles parse_output_files() const
    {
        BOOST_LOG_FUNCTION();
        auto prefix = vehlwn::invoke_with_error_context_str(
            [&] { return m_config.at_pointer("/output_files/prefix").as_string(); },
            "output_files.prefix key not found");

        auto extension = vehlwn::invoke_with_error_context_str(
            [&] {
                return m_config.at_pointer("/output_files/extension").as_string();
            },
            "output_files.extension key not found");
        if(extension.empty()) {
            BOOST_LOG_TRIVIAL(fatal) << "output_files.extension is empty";
            std::exit(1);
        }

        auto video_bitrate = [&]() -> std::optional<std::string> {
            const auto* tmp
                = m_config.at_pointer("/output_files/video_bitrate").if_string();
            if(tmp != nullptr) {
                return std::string(tmp->begin(), tmp->end());
            }
            return std::nullopt;
        }();
        auto audio_bitrate = [&]() -> std::optional<std::string> {
            const auto* tmp
                = m_config.at_pointer("/output_files/audio_bitrate").if_string();
            if(tmp != nullptr) {
                return std::string(tmp->begin(), tmp->end());
            }
            return std::nullopt;
        }();
        return {
            std::string(prefix.begin(), prefix.end()),
            std::string(extension.begin(), extension.end()),
            std::move(video_bitrate),
            std::move(audio_bitrate)};
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Logging parse_logging() const
    {
        std::string app_level = "info";
        if(auto tmp = m_config.at_pointer("/logging/app_level").if_string()) {
            app_level = {tmp->begin(), tmp->end()};
        }

        std::string ffmpeg_level = "warning";
        if(auto tmp = m_config.at_pointer("/logging/ffmpeg_level").if_string()) {
            ffmpeg_level = {tmp->begin(), tmp->end()};
        }
        return {std::move(app_level), std::move(ffmpeg_level)};
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::
        Knn
        parse_knn(const boost::json::object& back_subtr_obj) const
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
    [[nodiscard]] vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::
        Mog2
        parse_mog2(const boost::json::object& back_subtr_obj) const
    {
        BOOST_LOG_FUNCTION();
        auto ret = vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::
            Mog2();
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
    [[nodiscard]] vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor
        parse_background_subtractor(const boost::json::object& back_subtr_obj) const
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

    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess::NormalizedBox
        parse_normalized_filter() const
    {
        BOOST_LOG_FUNCTION();
        auto ret = vehlwn::ApplicationSettings::Preprocess::NormalizedBox();
        ret.kernel_size = vehlwn::invoke_with_error_context_str(
            [&] {
                auto tmp = m_config.at_pointer("/preprocess/smoothing/kernel_size")
                               .as_int64();
                if(tmp <= 0) {
                    BOOST_LOG_TRIVIAL(fatal) << "kernel_size must be positive";
                    std::exit(1);
                }
                return static_cast<int>(tmp);
            },
            "Invalid number for normalized kernel_size");
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess::Gaussian
        parse_gaussian_filter() const
    {
        BOOST_LOG_FUNCTION();
        auto ret = vehlwn::ApplicationSettings::Preprocess::Gaussian();
        ret.kernel_size = vehlwn::invoke_with_error_context_str(
            [&] {
                auto tmp = m_config.at_pointer("/preprocess/smoothing/kernel_size")
                               .as_int64();
                if(tmp <= 0 || tmp % 2 == 0) {
                    BOOST_LOG_TRIVIAL(fatal)
                        << "Gaussian kernel_size must be positive and odd";
                    std::exit(1);
                }
                return static_cast<int>(tmp);
            },
            "Invalid number format for Gaussian kernel_size");
        ret.sigma = [&] {
            if(const auto* tmp
               = m_config.at_pointer("/preprocess/smoothing/sigma").if_double()) {
                if(*tmp < 0.0) {
                    BOOST_LOG_TRIVIAL(fatal) << "Gaussian kernel standard deviation "
                                                "must non negative double";
                    std::exit(1);
                }
                return *tmp;
            }
            return 0.0;
        }();
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess::Median
        parse_median_filter() const
    {
        BOOST_LOG_FUNCTION();
        auto ret = vehlwn::ApplicationSettings::Preprocess::Median();
        ret.kernel_size = vehlwn::invoke_with_error_context_str(
            [&] {
                auto tmp = m_config.at_pointer("/preprocess/smoothing/kernel_size")
                               .as_int64();
                if(tmp <= 2 || tmp % 2 == 0) {
                    BOOST_LOG_TRIVIAL(fatal)
                        << "Median kernel_size must be odd and greater than 1";
                    std::exit(1);
                }
                return static_cast<int>(tmp);
            },
            "Invalid number format for median kernel_size");
        return ret;
    }
    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess parse_preprocess() const
    {
        BOOST_LOG_FUNCTION();
        auto ret = vehlwn::ApplicationSettings::Preprocess();
        ret.resize_factor = [&]() -> std::optional<double> {
            const auto* tmp
                = m_config.at_pointer("/preprocess/resize/factor").if_double();
            if(tmp != nullptr) {
                if(*tmp <= 0.0) {
                    BOOST_LOG_TRIVIAL(fatal)
                        << "preprocess.resize.factor must be positive double";
                    std::exit(1);
                }
                return *tmp;
            }
            return std::nullopt;
        }();

        auto algorithm_name = [&]() -> std::optional<std::string> {
            const auto* tmp
                = m_config.at_pointer("/preprocess/smoothing/algorithm").if_string();
            if(tmp != nullptr) {
                return std::string(tmp->begin(), tmp->end());
            }
            return std::nullopt;
        }();
        if(algorithm_name) {
            const auto value = algorithm_name.value();
            if(value == "normalized_box") {
                ret.smoothing = parse_normalized_filter();
            } else if(value == "gaussian") {
                ret.smoothing = parse_gaussian_filter();
            } else if(value == "median") {
                ret.smoothing = parse_median_filter();
            } else {
                BOOST_LOG_TRIVIAL(fatal)
                    << "Unknown smoothing filter algorithm: " << value;
                std::exit(1);
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
    const auto v = invoke_with_error_context_fun(
        [&] { return boost::json::parse(is); },
        [&] {
            std::ostringstream os;
            os << "Failed to parse config file " << config_path;
            return os.str();
        });

    const auto p = ConfigParser(v);
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
