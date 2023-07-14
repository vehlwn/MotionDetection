#pragma once

#include <optional>
#include <string>
#include <variant>

namespace vehlwn {
struct ApplicationSettings {
    struct VideoCapture {
        std::string filename;
        std::optional<std::string> file_format;
        std::optional<std::string> video_size;
        std::optional<std::string> framerate;
        std::optional<std::string> input_format;
    } video_capture;

    struct OutputFiles {
        std::string prefix;
        std::string extension;
        std::optional<std::string> video_bitrate;
        std::optional<std::string> audio_bitrate;
    } output_files;

    struct Logging {
        std::string app_level;
        std::string ffmpeg_level;
    } logging;

    struct Segmentation {
        struct BackgroundSubtractor {
            struct Knn {
                int history;
                double dist_2_threshold;
                bool detect_shadows;
            };
            struct Mog2 {
                int history;
                double var_threshold;
                bool detect_shadows;
            };
            std::variant<Knn, Mog2> algorithm;
        } background_subtractor;
        int min_moving_area{};
        double delta_without_motion{};
    } segmentation;

    struct Preprocess {
        std::optional<double> resize_factor;
        struct NormalizedBox {
            int kernel_size;
        };
        struct Gaussian {
            int kernel_size;
            double sigma;
        };
        struct Median {
            int kernel_size;
        };
        std::optional<std::variant<NormalizedBox, Gaussian, Median>> smoothing;
    } preprocess;
};

ApplicationSettings read_settings() noexcept;
} // namespace vehlwn
