#pragma once

#include "Poco/Util/AbstractConfiguration.h"

#include <string>

namespace vehlwn {
class ApplicationSettings
{
public:
    ApplicationSettings(const Poco::Util::AbstractConfiguration& config);
    std::string get_background_subtractor_algorithm() const;
    int get_background_subtractor_history(int default_value) const;
    double get_background_subtractor_dist_2_threshold(double default_value) const;
    bool get_background_subtractor_detect_shadows(bool default_value) const;
    double get_background_subtractor_var_threshold(double default_value) const;
    std::string get_video_capture_filename() const;
    std::string
        get_video_capture_api_preference(const std::string& default_value) const;
    bool has_video_capture_fourcc() const;
    std::string get_video_capture_fourcc() const;
    bool has_video_capture_frame_width() const;
    int get_video_capture_frame_width() const;
    bool has_video_capture_frame_height() const;
    int get_video_capture_frame_height() const;
    bool has_video_capture_fps() const;
    double get_video_capture_fps() const;
    std::string get_smoothing_filter_name() const;
    bool has_smoothing_filter_name() const;
    int get_smoothing_filter_kernel_size() const;
    double get_smoothing_filter_sigma(double default_value) const;

private:
    const Poco::Util::AbstractConfiguration& m_config;
};
} // namespace vehlwn
