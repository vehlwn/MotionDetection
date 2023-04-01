#pragma once

#include "Poco/Logger.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Util/AbstractConfiguration.h"

#include <array>
#include <optional>
#include <string>
#include <variant>

namespace vehlwn {
struct ApplicationSettings {
    Poco::Net::SocketAddress http_server_host_and_port;

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
    } output_files;

    struct BackgroundSubtractor {
        struct Knn {
            int history;
            double dist_2_threshold;
            bool detect_shadows;
            Knn()
                : history{}
                , dist_2_threshold{}
                , detect_shadows{}
            {}
        };
        struct Mog2 {
            int history;
            double var_threshold;
            bool detect_shadows;
            Mog2()
                : history{}
                , var_threshold{}
                , detect_shadows{}
            {}
        };
        std::variant<Knn, Mog2> algorithm;
    } background_subtractor;

    struct Preprocess {
        std::optional<double> resize_factor;
        struct NormalizedBox {
            int kernel_size;
            NormalizedBox()
                : kernel_size{}
            {}
        };
        struct Gaussian {
            int kernel_size;
            double sigma;
            Gaussian()
                : kernel_size{}
                , sigma{}
            {}
        };
        struct Median {
            int kernel_size;
            Median()
                : kernel_size{}
            {}
        };
        std::optional<std::variant<NormalizedBox, Gaussian, Median>> smoothing;
    } preprocess;
};

ApplicationSettings read_settings(
    const Poco::Util::AbstractConfiguration& config,
    Poco::Logger& logger) noexcept;
} // namespace vehlwn
