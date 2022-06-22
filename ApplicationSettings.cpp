#include "ApplicationSettings.h"

namespace {
static const std::string BACKGROUND_SUBTRACTOR_ALGORITHM_KEY =
    "background_subtractor.algorithm";
static const std::string BACKGROUND_SUBTRACTOR_HISTORY_KEY =
    "background_subtractor.history";
static const std::string BACKGROUND_SUBTRACTOR_DIST_2_THRESHOLD_KEY =
    "background_subtractor.dist_2_threshold";
static const std::string BACKGROUND_SUBTRACTOR_DETECT_SHADOWS_KEY =
    "background_subtractor.detect_shadows";
static const std::string BACKGROUND_SUBTRACTOR_VAR_THRESHOLD_KEY =
    "background_subtractor.var_threshold";
static const std::string VIDEO_CAPTURE_FILENAME_KEY = "video_capture.filename";
static const std::string VIDEO_CAPTURE_API_PREFERENCE_KEY =
    "video_capture.api_preference";
static const std::string VIDEO_CAPTURE_FOURCC_KEY = "video_capture.fourcc";
static const std::string VIDEO_CAPTURE_FRAME_WIDTH_KEY = "video_capture.frame_width";
static const std::string VIDEO_CAPTURE_FRAME_HEIGHT_KEY =
    "video_capture.frame_height";
static const std::string VIDEO_CAPTURE_FRAME_FPS_KEY = "video_capture.fps";
} // namespace

namespace vehlwn {
ApplicationSettings::ApplicationSettings(
    const Poco::Util::AbstractConfiguration& config)
    : m_config{config}
{
}

std::string ApplicationSettings::get_background_subtractor_algorithm() const
{
    return m_config.getString(BACKGROUND_SUBTRACTOR_ALGORITHM_KEY);
}

int ApplicationSettings::get_background_subtractor_history(int default_value) const
{
    return m_config.getInt(BACKGROUND_SUBTRACTOR_HISTORY_KEY, default_value);
}

double ApplicationSettings::get_background_subtractor_dist_2_threshold(
    double default_value) const
{
    return m_config.getDouble(
        BACKGROUND_SUBTRACTOR_DIST_2_THRESHOLD_KEY,
        default_value);
}

bool ApplicationSettings::get_background_subtractor_detect_shadows(
    bool default_value) const
{
    return m_config.getBool(BACKGROUND_SUBTRACTOR_DETECT_SHADOWS_KEY, default_value);
}

double ApplicationSettings::get_background_subtractor_var_threshold(
    double default_value) const
{
    return m_config.getDouble(
        BACKGROUND_SUBTRACTOR_VAR_THRESHOLD_KEY,
        default_value);
}

std::string ApplicationSettings::get_video_capture_filename() const
{
    return m_config.getString(VIDEO_CAPTURE_FILENAME_KEY);
}

std::string ApplicationSettings::get_video_capture_api_preference(
    const std::string& default_value) const
{
    return m_config.getString(VIDEO_CAPTURE_API_PREFERENCE_KEY, default_value);
}

bool ApplicationSettings::has_video_capture_fourcc() const
{
    return m_config.has(VIDEO_CAPTURE_FOURCC_KEY);
}

std::string ApplicationSettings::get_video_capture_fourcc() const
{
    return m_config.getString(VIDEO_CAPTURE_FOURCC_KEY);
}

bool ApplicationSettings::has_video_capture_frame_width() const
{
    return m_config.has(VIDEO_CAPTURE_FRAME_WIDTH_KEY);
}

int ApplicationSettings::get_video_capture_frame_width() const
{
    return m_config.getInt(VIDEO_CAPTURE_FRAME_WIDTH_KEY);
}

bool ApplicationSettings::has_video_capture_frame_height() const
{
    return m_config.has(VIDEO_CAPTURE_FRAME_HEIGHT_KEY);
}

int ApplicationSettings::get_video_capture_frame_height() const
{
    return m_config.getInt(VIDEO_CAPTURE_FRAME_HEIGHT_KEY);
}

bool ApplicationSettings::has_video_capture_fps() const
{
    return m_config.has(VIDEO_CAPTURE_FRAME_HEIGHT_KEY);
}

double ApplicationSettings::get_video_capture_fps() const
{
    return m_config.getDouble(VIDEO_CAPTURE_FRAME_HEIGHT_KEY);
}
} // namespace vehlwn
