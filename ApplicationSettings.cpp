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

const auto DEFAULT_SETTINGS = [] {
    std::map<QString, QVariant> result;
    result[CAMERA_CHECKED_ENTRY] = true;
    result[FILE_CHECKED_ENTRY] = false;
    result[CAMERA_INDEX_ENTRY] = 0;
    result[FNAME_ENTRY] = "";
    return result;
}();

QVariant getSettingsValue(QSettings& s, const QString& key, const QVariant& def = {})
{
    if(!s.contains(key))
        s.setValue(key, def);
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
    const auto key = CAMERA_CHECKED_ENTRY;
    return getSettingsValue(pimpl->settings, key, DEFAULT_SETTINGS.at(key)).toBool();
}

void ApplicationSettings::cameraChecked(bool b)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(CAMERA_CHECKED_ENTRY, b);
}

bool ApplicationSettings::fileChecked() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    const auto key = FILE_CHECKED_ENTRY;
    return getSettingsValue(pimpl->settings, key, DEFAULT_SETTINGS.at(key)).toBool();
}

void ApplicationSettings::fileChecked(bool b)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(FILE_CHECKED_ENTRY, b);
}

int ApplicationSettings::cameraIndex() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    const auto key = CAMERA_INDEX_ENTRY;
    return getSettingsValue(pimpl->settings, key, DEFAULT_SETTINGS.at(key)).toInt();
}

void ApplicationSettings::cameraIndex(int i)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(CAMERA_INDEX_ENTRY, i);
}

QString ApplicationSettings::fname() const
{
    QMutexLocker lock{&pimpl->settingsMutex};
    const auto key = FNAME_ENTRY;
    return getSettingsValue(pimpl->settings, key, DEFAULT_SETTINGS.at(key))
        .toString();
}

void ApplicationSettings::fname(QString s)
{
    QMutexLocker lock{&pimpl->settingsMutex};
    pimpl->settings.setValue(FNAME_ENTRY, s);
}