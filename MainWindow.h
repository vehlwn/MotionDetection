#pragma once

#include <QMainWindow>
#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    void connectUiSlots();
};
