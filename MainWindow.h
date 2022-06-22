#pragma once

#include <QMainWindow>
#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void logMessage(QString s);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
