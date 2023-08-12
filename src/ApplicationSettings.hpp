#pragma once

#include <map>
#include <optional>
#include <string>
#include <variant>

namespace vehlwn {
struct ApplicationSettings {
    struct VideoCapture {
        std::string filename;
        std::optional<std::string> file_format;
        std::map<std::string, std::string> demuxer_options;

        struct VideoDecoder {
            std::optional<std::string> hw_type;
        };
        std::optional<VideoDecoder> video_decoder;
    } video_capture;

    struct OutputFiles {
        std::string prefix;
        std::string extension;
        std::optional<std::string> video_bitrate;
        std::optional<std::string> audio_bitrate;

        struct VideoEncoder {
            std::string codec_name;
            std::optional<std::string> hw_type;
            std::map<std::string, std::string> private_options;
        };
        VideoEncoder video_encoder;
    } output_files;

    struct Logging {
        std::string app_level;
        std::string ffmpeg_level;
        bool show_timestamp;
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
        bool convert_to_gray{};
        std::optional<double> resize_factor;
        struct Smoothing {
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
            std::variant<NormalizedBox, Gaussian, Median> algorithm;
        };
        std::optional<Smoothing> smoothing;
    } preprocess;
};

ApplicationSettings read_settings() noexcept;
} // namespace vehlwn
