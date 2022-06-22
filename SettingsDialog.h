#pragma once

#include <QDialog>
#include <QString>
#include <memory>

class SettingsDialog : public QDialog
{
    Q_OBJECT
    using base = QDialog;

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

    void accept() override;

private slots:
    bool validateGaussianBlurValue(int value) ;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    void setErrorText(QString s);
    bool valudateForm();
};
