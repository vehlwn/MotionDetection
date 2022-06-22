#include "SettingsDialog.h"

#include "ApplicationSettings.h"
#include "ui_SettingsDialog.h"

#include <QDebug>
#include <QFileDialog>

namespace Ui {
class SettingsDialog;
}

struct SettingsDialog::Impl
{
    Ui::SettingsDialog ui;
};

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->ui.setupUi(this);
    const auto radioCameraIndexToggled = [this](bool b) {
        pimpl->ui.spinBoxCameraIndex->setEnabled(b);
    };
    const auto radioFileToggled = [this](bool b) {
        pimpl->ui.lineEditFname->setEnabled(b);
        pimpl->ui.pushButtonOpenFile->setEnabled(b);
    };
    connect(
        pimpl->ui.radioButtonCamera,
        &QAbstractButton::toggled,
        this,
        radioCameraIndexToggled);
    connect(
        pimpl->ui.radioButtonFile,
        &QAbstractButton::toggled,
        this,
        radioFileToggled);
    connect(pimpl->ui.pushButtonOpenFile, &QAbstractButton::clicked, this, [this]() {
        const QString fileName = QFileDialog::getOpenFileName(
            this,
            "Open video file",
            ApplicationSettings::i().fname(),
            "All files (*.*);;AVI files (*.avi);;MP4 files (*.mp4);;MKV files "
            "(*.mkv)");

        if(fileName.isEmpty())
            return;
        pimpl->ui.lineEditFname->setText(fileName);
        ApplicationSettings::i().fname(fileName);
    });
    const auto& i = ApplicationSettings::i();
    pimpl->ui.radioButtonCamera->setChecked(i.cameraChecked());
    radioCameraIndexToggled(i.cameraChecked());
    pimpl->ui.radioButtonFile->setChecked(i.fileChecked());
    radioFileToggled(i.fileChecked());
    pimpl->ui.spinBoxCameraIndex->setValue(i.cameraIndex());
    pimpl->ui.lineEditFname->setText(i.fname());
    pimpl->ui.spinBoxHistory->setValue(i.history());
    pimpl->ui.spinBoxFrameBufferSize->setValue(i.frameBufferSize());
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::accept()
{
    auto& i = ApplicationSettings::i();
    i.cameraChecked(pimpl->ui.radioButtonCamera->isChecked());
    i.fileChecked(pimpl->ui.radioButtonFile->isChecked());
    i.cameraIndex(pimpl->ui.spinBoxCameraIndex->value());
    i.fname(pimpl->ui.lineEditFname->text());
    i.history(pimpl->ui.spinBoxHistory->value());
    i.frameBufferSize(pimpl->ui.spinBoxFrameBufferSize->value());
    base::accept();
}
