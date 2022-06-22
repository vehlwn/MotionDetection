#include "ApplicationSettings.h"

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <map>

struct ApplicationSettings::Impl
{
    QSettings settings;
    QMutex settingsMutex;

    template<class... Args>
    Impl(Args... args)
        : settings{std::move(args)...}
    {
    }
};

namespace {
constexpr auto SETTINGS_FILE_NAME = "MotionDetection.ini";
constexpr auto CAMERA_CHECKED_ENTRY = "camera_checked";
constexpr auto FILE_CHECKED_ENTRY = "file_checked";
constexpr auto CAMERA_INDEX_ENTRY = "camera_index";
constexpr auto FNAME_ENTRY = "fname";
constexpr auto HISTORY_ENTRY = "history";
constexpr auto FRAME_BUFFER_SIZE_ENTRY = "frame_buffer_size";
constexpr auto OUTPUT_FOLDER_ENTRY = "output_folder";
constexpr auto OUTPUT_EXTENSION_ENTRY = "output_extension";
constexpr auto GAUSSIAN_BLUR_CHECKED_ENTRY = "gaussian_blur_checked";
constexpr auto GAUSSIAN_BLUR_VALUE_ENTRY = "gaussian_blur_value";
constexpr auto FILE_ROTATION_PERIOD_VALUE_ENTRY = "file_rotation_period_value";
constexpr auto FILE_ROTATION_PERIOD_UNIT_ENTRY = "file_rotation_period_unit";
constexpr auto MIN_MOVING_AREA_ENTRY = "min_moving_area";
constexpr auto DELTA_WITHOUT_MOTION_ENTRY = "delta_without_motion";

const auto DEFAULT_SETTINGS = [] {
    std::map<QString, QVariant> result;
    result[CAMERA_CHECKED_ENTRY] = true;
    result[FILE_CHECKED_ENTRY] = false;
    result[CAMERA_INDEX_ENTRY] = 0;
    result[FNAME_ENTRY] = "";
    result[HISTORY_ENTRY] = 100;
    result[FRAME_BUFFER_SIZE_ENTRY] = 10;
    result[OUTPUT_FOLDER_ENTRY] = "video";
    result[OUTPUT_EXTENSION_ENTRY] = ".avi";
    result[GAUSSIAN_BLUR_CHECKED_ENTRY] = true;
    result[GAUSSIAN_BLUR_VALUE_ENTRY] = 5;
    result[FILE_ROTATION_PERIOD_VALUE_ENTRY] = 1.0;
    result[FILE_ROTATION_PERIOD_UNIT_ENTRY] = "h";
    result[MIN_MOVING_AREA_ENTRY] = 500;
    result[DELTA_WITHOUT_MOTION_ENTRY] = 5.0;
    return result;
}();

QVariant getSettingsValue(QSettings& s, const QString& key)
{
    if(!s.contains(key))
        s.setValue(key, DEFAULT_SETTINGS.at(key));
    return s.value(key);
}

} // namespace

ApplicationSettings::ApplicationSettings()
    : pimpl{std::make_unique<Impl>(SETTINGS_FILE_NAME, QSettings::IniFormat)}
{
}

ApplicationSettings::~ApplicationSettings() = default;

ApplicationSettings& ApplicationSettings::i()
{
    static ApplicationSettings obj;
    return obj;
}

bool ApplicationSettings::cameraChecked() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, CAMERA_CHECKED_ENTRY).toBool();
}

void ApplicationSettings::cameraChecked(bool b)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(CAMERA_CHECKED_ENTRY, b);
}

bool ApplicationSettings::fileChecked() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, FILE_CHECKED_ENTRY).toBool();
}

void ApplicationSettings::fileChecked(bool b)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(FILE_CHECKED_ENTRY, b);
}

int ApplicationSettings::cameraIndex() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, CAMERA_INDEX_ENTRY).toInt();
}

void ApplicationSettings::cameraIndex(int i)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(CAMERA_INDEX_ENTRY, i);
}

QString ApplicationSettings::fname() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, FNAME_ENTRY).toString();
}

void ApplicationSettings::fname(QString s)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(FNAME_ENTRY, s);
}

int ApplicationSettings::history() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, HISTORY_ENTRY).toInt();
}

void ApplicationSettings::history(int i)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(HISTORY_ENTRY, i);
}

int ApplicationSettings::frameBufferSize() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, FRAME_BUFFER_SIZE_ENTRY).toInt();
}

void ApplicationSettings::frameBufferSize(int i)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(FRAME_BUFFER_SIZE_ENTRY, i);
}

QString ApplicationSettings::outputFolder() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, OUTPUT_FOLDER_ENTRY).toString();
}

void ApplicationSettings::outputFolder(QString s)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(OUTPUT_FOLDER_ENTRY, s);
}

QString ApplicationSettings::outputExtension() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, OUTPUT_EXTENSION_ENTRY).toString();
}

void ApplicationSettings::outputExtension(QString s)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(OUTPUT_EXTENSION_ENTRY, s);
}

bool ApplicationSettings::gaussianBlurChecked() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, GAUSSIAN_BLUR_CHECKED_ENTRY).toBool();
}

void ApplicationSettings::gaussianBlurChecked(bool b)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(GAUSSIAN_BLUR_CHECKED_ENTRY, b);
}

int ApplicationSettings::gaussianBlurValue() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, GAUSSIAN_BLUR_VALUE_ENTRY).toInt();
}

void ApplicationSettings::gaussianBlurValue(int i)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(GAUSSIAN_BLUR_VALUE_ENTRY, i);
}

double ApplicationSettings::fileRotationPeriodValue() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, FILE_ROTATION_PERIOD_VALUE_ENTRY)
        .toDouble();
}

void ApplicationSettings::fileRotationPeriodValue(double d)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(FILE_ROTATION_PERIOD_VALUE_ENTRY, d);
}

QString ApplicationSettings::fileRotationPeriodUnit() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, FILE_ROTATION_PERIOD_UNIT_ENTRY)
        .toString();
}

void ApplicationSettings::fileRotationPeriodUnit(QString s)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(FILE_ROTATION_PERIOD_UNIT_ENTRY, s);
}

double ApplicationSettings::fileRotationMsec() const
{
    const double rotationValue = fileRotationPeriodValue();
    const QString rotationUnit = fileRotationPeriodUnit();
    if(rotationUnit == "s")
        return rotationValue * 1000.;
    else if(rotationUnit == "min")
        return rotationValue * 1000. * 60.;
    else if(rotationUnit == "h")
        return rotationValue * 1000. * 60. * 60.;
    return 1000. * 60. * 60.;
}

std::vector<QString> ApplicationSettings::validFileRotationUnits() const
{
    return {"s", "min", "h"};
}

int ApplicationSettings::minMovingArea() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, MIN_MOVING_AREA_ENTRY).toInt();
}

void ApplicationSettings::minMovingArea(int i)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(MIN_MOVING_AREA_ENTRY, i);
}

double ApplicationSettings::deltaWithoutMotion() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    return getSettingsValue(pimpl->settings, DELTA_WITHOUT_MOTION_ENTRY).toDouble();
}

void ApplicationSettings::deltaWithoutMotion(double d)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(DELTA_WITHOUT_MOTION_ENTRY, d);
}
