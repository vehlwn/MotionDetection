#pragma once

#include <QString>
#include <memory>

class ApplicationSettings
{
public:
    ApplicationSettings(const ApplicationSettings&) = delete;
    ApplicationSettings(ApplicationSettings&&) = delete;
    ApplicationSettings& operator=(const ApplicationSettings&) = delete;
    ApplicationSettings& operator=(ApplicationSettings&&) = delete;
    ~ApplicationSettings();

    static ApplicationSettings& i();

    bool cameraChecked() const;
    void cameraChecked(bool b);
    bool fileChecked() const;
    void fileChecked(bool b);
    int cameraIndex() const;
    void cameraIndex(int i);
    QString fname() const;
    void fname(QString s);

private:
    ApplicationSettings();
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};