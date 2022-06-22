#pragma once

#include <QString>
#include <memory>
#include <vector>

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
    int history() const;
    void history(int i);
    int frameBufferSize() const;
    void frameBufferSize(int i);
    QString outputFolder() const;
    void outputFolder(QString s);
    QString outputExtension() const;
    void outputExtension(QString s);
    bool gaussianBlurChecked() const;
    void gaussianBlurChecked(bool b);
    int gaussianBlurValue() const;
    void gaussianBlurValue(int i);
    double fileRotationPeriodValue() const;
    void fileRotationPeriodValue(double d);
    QString fileRotationPeriodUnit() const;
    void fileRotationPeriodUnit(QString s);
    double fileRotationMsec() const;
    std::vector<QString> validFileRotationUnits() const;
    int minMovingArea() const;
    void minMovingArea(int i);
    double deltaWithoutMotion() const;
    void deltaWithoutMotion(double d);

private:
    ApplicationSettings();
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
