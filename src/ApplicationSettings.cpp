#include "ApplicationSettings.hpp"

#include <cstdlib>
#include <exception>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/trivial.hpp>

#include "Config.hpp"
#include "ErrorWithContext.hpp"
#include "ini/src/Parser.hpp"

namespace {

vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::Knn
    parse_knn(const vehlwn::ini::Section& back_subtr_obj)
{
    auto ret
        = vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::Knn();
    ret.history = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = back_subtr_obj.get("history")) {
                const auto tmp = it->get_number<int>();
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
            if(const auto it = back_subtr_obj.get("dist_2_threshold")) {
                const auto tmp = it->get_number<double>();
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
            if(const auto it = back_subtr_obj.get("detect_shadows")) {
                const auto tmp = it->get_bool();
                return tmp;
            }
            return false;
        },
        "Failed to parse background_subtractor.detect_shadows");
    return ret;
}

vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::Mog2
    parse_mog2(const vehlwn::ini::Section& back_subtr_obj)
{
    auto ret
        = vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor::Mog2();
    ret.history = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = back_subtr_obj.get("history")) {
                const auto tmp = it->get_number<int>();
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
            if(const auto it = back_subtr_obj.get("var_threshold")) {
                const auto tmp = it->get_number<double>();
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
            if(const auto it = back_subtr_obj.get("detect_shadows")) {
                const auto tmp = it->get_bool();
                return tmp;
            }
            return false;
        },
        "Failed to parse background_subtractor.detect_shadows");
    return ret;
}

vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor
    parse_background_subtractor(const vehlwn::ini::Section& back_subtr_obj)
{
    const auto algorithm_name = [&] {
        if(const auto it = back_subtr_obj.get("algorithm")) {
            return std::string(it->get_string_view());
        }
        throw std::runtime_error("background_subtractor.algorithm key not found");
    }();

    auto ret = vehlwn::ApplicationSettings::Segmentation::BackgroundSubtractor();
    // https://docs.opencv.org/4.5.3/de/de1/group__video__motion.html
    if(algorithm_name == "KNN") {
        ret.algorithm = vehlwn::invoke_with_error_context_str(
            [&] { return parse_knn(back_subtr_obj); },
            "Failed to parse knn algorithm");
    } else if(algorithm_name == "MOG2") {
        ret.algorithm = vehlwn::invoke_with_error_context_str(
            [&] { return parse_mog2(back_subtr_obj); },
            "Failed to parse mog2 algorithm");
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
    parse_normalized_filter(const vehlwn::ini::Section& smooth_obj)
{
    auto ret = vehlwn::ApplicationSettings::Preprocess::Smoothing::NormalizedBox();
    ret.kernel_size = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = smooth_obj.get("kernel_size")) {
                const auto tmp = it->get_number<int>();
                if(tmp <= 0) {
                    throw std::runtime_error("kernel_size must be positive");
                }
                return tmp;
            }
            throw std::runtime_error("kernel_size key not found");
        },
        "Failed to parse kernel_size");
    return ret;
}

vehlwn::ApplicationSettings::Preprocess::Smoothing::Gaussian
    parse_gaussian_filter(const vehlwn::ini::Section& smooth_obj)
{
    auto ret = vehlwn::ApplicationSettings::Preprocess::Smoothing::Gaussian();
    ret.kernel_size = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = smooth_obj.get("kernel_size")) {
                const auto tmp = it->get_number<int>();
                if(tmp <= 0 || tmp % 2 == 0) {
                    throw std::runtime_error(
                        "Gaussian kernel_size must be positive and odd");
                }
                return tmp;
            }
            throw std::runtime_error("kernel_size key not found");
        },
        "Failed to parse kernel_size");
    ret.sigma = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = smooth_obj.get("sigma")) {
                const auto tmp = it->get_number<double>();
                if(tmp < 0) {
                    throw std::runtime_error("Gaussian kernel standard deviation "
                                             "must non negative double");
                }
                return tmp;
            }
            return 0.0;
        },
        "Failed to parse kernel_size");
    return ret;
}

vehlwn::ApplicationSettings::Preprocess::Smoothing::Median
    parse_median_filter(const vehlwn::ini::Section& smooth_obj)
{
    auto ret = vehlwn::ApplicationSettings::Preprocess::Smoothing::Median();
    ret.kernel_size = vehlwn::invoke_with_error_context_str(
        [&] {
            if(const auto it = smooth_obj.get("kernel_size")) {
                const auto tmp = it->get_number<int>();
                if(tmp <= 0 || tmp % 2 == 0) {
                    throw std::runtime_error(
                        "Gaussian kernel_size must be positive and odd");
                }
                return tmp;
            }
            throw std::runtime_error("kernel_size key not found");
        },
        "Failed to parse kernel_size");
    return ret;
}

vehlwn::ApplicationSettings::Preprocess::Smoothing
    parse_smoothing(const vehlwn::ini::Section& smooth_obj)
{
    const auto algorithm_name = [&] {
        if(const auto tmp = smooth_obj.get("algorithm")) {
            return std::string(tmp->get_string_view());
        }
        throw std::runtime_error("algorithm key not found");
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
    vehlwn::ini::Ini m_config;

public:
    explicit ConfigParser(vehlwn::ini::Ini&& config)
        : m_config(std::move(config))
    {}

    [[nodiscard]] vehlwn::ApplicationSettings::VideoCapture
        parse_video_capture() const
    {
        const auto video_cap_obj = [&] {
            if(auto opt = m_config.section("video_capture")) {
                return *opt;
            }
            throw std::runtime_error("video_capture section not found");
        }();
        auto filename = [&] {
            if(auto opt = video_cap_obj.get("filename")) {
                return std::string(opt->get_string_view());
            }
            throw std::runtime_error("video_capture.filename key not found");
        }();
        auto file_format = [&]() -> std::optional<std::string> {
            if(auto opt = video_cap_obj.get("file_format")) {
                return std::string(opt->get_string_view());
            }
            return std::nullopt;
        }();
        auto video_size = [&]() -> std::optional<std::string> {
            if(auto opt = video_cap_obj.get("video_size")) {
                return std::string(opt->get_string_view());
            }
            return std::nullopt;
        }();
        auto framerate = [&]() -> std::optional<std::string> {
            if(auto opt = video_cap_obj.get("framerate")) {
                return std::string(opt->get_string_view());
            }
            return std::nullopt;
        }();
        auto input_format = [&]() -> std::optional<std::string> {
            if(auto opt = video_cap_obj.get("input_format")) {
                return std::string(opt->get_string_view());
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
        const auto out_file_obj = [&] {
            if(auto opt = m_config.section("output_files")) {
                return *opt;
            }
            throw std::runtime_error("output_files section not found");
        }();
        auto prefix = [&] {
            if(auto tmp = out_file_obj.get("prefix")) {
                return std::string(tmp->get_string_view());
            }
            throw std::runtime_error("output_files.prefix key not found");
        }();
        auto extension = [&] {
            if(auto tmp = out_file_obj.get("extension")) {
                if(tmp->get_string_view().empty()) {
                    throw std::runtime_error(
                        "output_files.extension cannot be empty");
                }
                return std::string(tmp->get_string_view());
            }
            throw std::runtime_error("output_files.extension key not found");
        }();
        auto video_bitrate = [&]() -> std::optional<std::string> {
            if(auto tmp = out_file_obj.get("video_bitrate")) {
                return std::string(tmp->get_string_view());
            }
            return std::nullopt;
        }();
        auto audio_bitrate = [&]() -> std::optional<std::string> {
            if(auto tmp = out_file_obj.get("audio_bitrate")) {
                return std::string(tmp->get_string_view());
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
        auto ret = vehlwn::ApplicationSettings::Logging();
        ret.app_level = "info";
        ret.ffmpeg_level = "warning";
        ret.show_timestamp = true;

        if(const auto logging_obj = m_config.section("logging")) {
            if(const auto tmp = logging_obj->get("app_level")) {
                ret.app_level = std::string(tmp->get_string_view());
            }
            if(const auto tmp = logging_obj->get("ffmpeg_level")) {
                ret.ffmpeg_level = std::string(tmp->get_string_view());
            }
            if(const auto tmp = logging_obj->get("show_timestamp")) {
                ret.show_timestamp = vehlwn::invoke_with_error_context_str(
                    [&] { return tmp->get_bool(); },
                    "Failed to parse logging.show_timestamp");
            }
        }
        return ret;
    }

    [[nodiscard]] vehlwn::ApplicationSettings::Segmentation
        parse_segmentation() const
    {
        auto ret = vehlwn::ApplicationSettings::Segmentation();

        const auto back_subtr_obj = [&] {
            if(auto tmp = m_config.section("segmentation.background_subtractor")) {
                return *tmp;
            }
            throw std::runtime_error(
                "segmentation.background_subtractor section not found");
        }();

        ret.background_subtractor = vehlwn::invoke_with_error_context_str(
            [&] { return parse_background_subtractor(back_subtr_obj); },
            "Failed to parse segmentation.background_subtractor");

        const auto segm_obj = m_config.section("segmentation");
        ret.min_moving_area = 500;
        ret.delta_without_motion = 5.;
        if(segm_obj) {
            if(const auto it = segm_obj->get("min_moving_area")) {
                ret.min_moving_area = vehlwn::invoke_with_error_context_str(
                    [&] {
                        const auto tmp = it->get_number<int>();
                        if(tmp < 0) {
                            throw std::runtime_error(
                                "min_moving_area cannot be negative");
                        }
                        return tmp;
                    },
                    "Failed to parse segmentation.min_moving_area");
            }
            if(const auto it = segm_obj->get("delta_without_motion")) {
                ret.delta_without_motion = vehlwn::invoke_with_error_context_str(
                    [&] {
                        const auto tmp = it->get_number<double>();
                        if(tmp < 0.) {
                            throw std::runtime_error(
                                "delta_without_motion cannot be negative");
                        }
                        return tmp;
                    },
                    "Failed to parse segmentation.delta_without_motion");
            }
        }
        return ret;
    }

    [[nodiscard]] vehlwn::ApplicationSettings::Preprocess parse_preprocess() const
    {
        auto ret = vehlwn::ApplicationSettings::Preprocess();
        if(const auto preprocess_obj = m_config.section("preprocess")) {
            ret.convert_to_gray = vehlwn::invoke_with_error_context_str(
                [&] {
                    if(const auto it = preprocess_obj->get("convert_to_gray")) {
                        const auto tmp = it->get_bool();
                        return tmp;
                    }
                    return false;
                },
                "Failed to parse preprocess.convert_to_gray");
            ret.resize_factor = vehlwn::invoke_with_error_context_str(
                [&]() -> std::optional<double> {
                    if(const auto it = preprocess_obj->get("resize_factor")) {
                        const auto tmp = it->get_number<double>();
                        if(tmp <= 0) {
                            throw std::runtime_error(
                                "preprocess.resize_factor must be positive double");
                        }
                        return tmp;
                    }
                    return std::nullopt;
                },
                "Failed to parse preprocess.resize_factor");
        }
        if(const auto smooth_obj = m_config.section("preprocess.smoothing")) {
            ret.smoothing = vehlwn::invoke_with_error_context_str(
                [&] { return parse_smoothing(smooth_obj.value()); },
                "Failed to parse preprocess.smoothing");
        }
        return ret;
    }
};

} // namespace

namespace vehlwn {
namespace {
constexpr std::string_view CONFIG_FILE_NAME = "app.ini";
}

ApplicationSettings read_settings() noexcept
try {
    BOOST_LOG_FUNCTION();
    const auto config_path
        = std::string(CONFIG_DIR) + "/" + std::string(CONFIG_FILE_NAME);
    auto is = [&] {
        if(auto ret = std::ifstream(config_path, std::ios::binary)) {
            return ret;
        }
        throw std::runtime_error("Failed to open config file " + config_path);
    }();
    auto v = invoke_with_error_context_fun(
        [&] { return ini::parser::parse(is); },
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
