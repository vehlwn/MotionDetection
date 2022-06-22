#include "SettingsDialog.h"

#include "ApplicationSettings.h"
#include "ui_SettingsDialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QTimer>

namespace {
constexpr auto GAUSSIAN_BLUR_VALUE_ERR =
    "Gaussian kernel size must be positive odd integer";
constexpr auto FOURCC_VALUE_ERR = "FOURCC must be 4 characters";
}; // namespace

namespace Ui {
class SettingsDialog;
}

struct SettingsDialog::Impl
{
    Ui::SettingsDialog ui;
    QTimer errorMessageCleainig;
};

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->ui.setupUi(this);
    connect(&pimpl->errorMessageCleainig, &QTimer::timeout, this, [this] {
        pimpl->ui.labelError->clear();
    });
    pimpl->errorMessageCleainig.setSingleShot(true);
    setErrorText("");

    const auto radioCameraIndexToggled = [this](bool b) {
        pimpl->ui.spinBoxCameraIndex->setEnabled(b);
    };
    const auto radioFileToggled = [this](bool b) {
        pimpl->ui.lineEditFname->setEnabled(b);
        pimpl->ui.pushButtonOpenFile->setEnabled(b);
    };
    const auto gaussianBlurToggled = [this](bool b) {
        pimpl->ui.spinBoxGaussianBlur->setEnabled(b);
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
    connect(
        pimpl->ui.pushButtonOpenOutputFolder,
        &QAbstractButton::clicked,
        this,
        [this]() {
            const QString folderName = QFileDialog::getExistingDirectory(
                this,
                "Open output folder",
                ApplicationSettings::i().outputFolder());
            if(folderName.isEmpty())
                return;
            pimpl->ui.lineEditOutputFolder->setText(folderName);
            ApplicationSettings::i().outputFolder(folderName);
        });
    const auto& i = ApplicationSettings::i();
    pimpl->ui.lineEditSettingsFileName->setText(i.settingsFileName());
    for(auto s : i.validExtensions())
        pimpl->ui.comboBoxOutputExtension->addItem(s);
    for(auto s : i.validFileRotationUnits())
        pimpl->ui.comboBoxFileRotationUnit->addItem(s);
    for(auto s : i.exampleOutputFourCC())
        pimpl->ui.comboBoxOutputFourCC->addItem(s);
    connect(
        pimpl->ui.checkBoxGaussianBlur,
        &QAbstractButton::toggled,
        this,
        gaussianBlurToggled);
    connect(
        pimpl->ui.spinBoxGaussianBlur,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        &SettingsDialog::validateGaussianBlurValue);
    connect(
        pimpl->ui.comboBoxOutputFourCC,
        &QComboBox::currentTextChanged,
        this,
        &SettingsDialog::validateOutputFourCC);
    pimpl->ui.radioButtonCamera->setChecked(i.cameraChecked());
    radioCameraIndexToggled(i.cameraChecked());
    pimpl->ui.radioButtonFile->setChecked(i.fileChecked());
    radioFileToggled(i.fileChecked());
    pimpl->ui.spinBoxCameraIndex->setValue(i.cameraIndex());
    pimpl->ui.lineEditFname->setText(i.fname());
    pimpl->ui.spinBoxHistory->setValue(i.history());
    pimpl->ui.spinBoxFrameBufferSize->setValue(i.frameBufferSize());
    pimpl->ui.lineEditOutputFolder->setText(i.outputFolder());
    pimpl->ui.comboBoxOutputExtension->setCurrentText(i.outputExtension());
    pimpl->ui.checkBoxGaussianBlur->setChecked(i.gaussianBlurChecked());
    gaussianBlurToggled(i.gaussianBlurChecked());
    pimpl->ui.spinBoxGaussianBlur->setValue(i.gaussianBlurValue());
    pimpl->ui.doubleSpinBoxFileRotation->setValue(i.fileRotationPeriodValue());
    pimpl->ui.comboBoxFileRotationUnit->setCurrentText(i.fileRotationPeriodUnit());
    pimpl->ui.spinBoxMinMovingArea->setValue(i.minMovingArea());
    pimpl->ui.doubleSpinBoxDeltaWithoutMotion->setValue(i.deltaWithoutMotion());
    pimpl->ui.comboBoxOutputFourCC->setCurrentText(i.outputFourCC());
    pimpl->ui.spinBoxRecommendedInputWidth->setValue(i.recommendedInputWidth());
    pimpl->ui.spinBoxRecommendedInputHeight->setValue(i.recommendedInputHeight());
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::accept()
{
    if(valudateForm())
    {
        auto& i = ApplicationSettings::i();
        i.cameraChecked(pimpl->ui.radioButtonCamera->isChecked());
        i.fileChecked(pimpl->ui.radioButtonFile->isChecked());
        i.cameraIndex(pimpl->ui.spinBoxCameraIndex->value());
        i.fname(pimpl->ui.lineEditFname->text());
        i.history(pimpl->ui.spinBoxHistory->value());
        i.frameBufferSize(pimpl->ui.spinBoxFrameBufferSize->value());
        i.outputFolder(pimpl->ui.lineEditOutputFolder->text());
        i.outputExtension(pimpl->ui.comboBoxOutputExtension->currentText());
        i.gaussianBlurChecked(pimpl->ui.checkBoxGaussianBlur->isChecked());
        i.gaussianBlurValue(pimpl->ui.spinBoxGaussianBlur->value());
        i.fileRotationPeriodValue(pimpl->ui.doubleSpinBoxFileRotation->value());
        i.fileRotationPeriodUnit(pimpl->ui.comboBoxFileRotationUnit->currentText());
        i.minMovingArea(pimpl->ui.spinBoxMinMovingArea->value());
        i.deltaWithoutMotion(pimpl->ui.doubleSpinBoxDeltaWithoutMotion->value());
        i.outputFourCC(pimpl->ui.comboBoxOutputFourCC->currentText());
        i.recommendedInputWidth(pimpl->ui.spinBoxRecommendedInputWidth->value());
        i.recommendedInputHeight(pimpl->ui.spinBoxRecommendedInputHeight->value());
        base::accept();
    }
}

void SettingsDialog::setErrorText(QString s)
{
    pimpl->ui.labelError->setText(s);
    pimpl->errorMessageCleainig.start(5000);
}

bool SettingsDialog::validateGaussianBlurValue(int value)
{
    bool ok =
        !pimpl->ui.checkBoxGaussianBlur->isChecked() || value % 2 == 1 && value > 0;
    if(ok)
        setErrorText("");
    else
        setErrorText(GAUSSIAN_BLUR_VALUE_ERR);
    return ok;
}

bool SettingsDialog::validateOutputFourCC(const QString& text)
{
    bool ok = text.size() == 4;
    if(ok)
        setErrorText("");
    else
        setErrorText(FOURCC_VALUE_ERR);
    return ok;
}

bool SettingsDialog::valudateForm()
{
    if(!validateGaussianBlurValue(pimpl->ui.spinBoxGaussianBlur->value()))
    {
        pimpl->ui.spinBoxGaussianBlur->setFocus();
        return false;
    }
    if(!validateOutputFourCC(pimpl->ui.comboBoxOutputFourCC->currentText()))
    {
        pimpl->ui.comboBoxOutputFourCC->setFocus();
        return false;
    }
    return true;
}
