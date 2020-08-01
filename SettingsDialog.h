#pragma once

#include <QDialog>
#include <memory>

class SettingsDialog : public QDialog
{
    Q_OBJECT
    using base = QDialog;

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

    void accept() override;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
