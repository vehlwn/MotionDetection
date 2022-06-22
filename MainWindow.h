#pragma once

#include "FrameProducerThread.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui{};
    QGraphicsScene* m_scene{};
    QGraphicsPixmapItem* m_scenePixmapItem{};
    std::unique_ptr<FrameProducerThread> m_frameProducerThread;
};
