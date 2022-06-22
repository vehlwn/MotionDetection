#include "MainWindow.h"

#include "FrameProducerThread.h"
#include "ui_MainWindow.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

struct MainWindow::Impl
{
    Ui::MainWindow ui;
    QGraphicsScene* scene{};
    QGraphicsPixmapItem* scenePixmapItem{};
    std::unique_ptr<FrameProducerThread> frameProducerThread;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->scene = new QGraphicsScene;
    pimpl->scenePixmapItem = new QGraphicsPixmapItem;
    pimpl->frameProducerThread = std::make_unique<FrameProducerThread>();
    pimpl->ui.setupUi(this);

    pimpl->ui.graphicsView->setScene(pimpl->scene);
    pimpl->scene->addItem(pimpl->scenePixmapItem);
    connect(
        pimpl->frameProducerThread.get(),
        &FrameProducerThread::newFrame,
        this,
        [this](QPixmap img) { pimpl->scenePixmapItem->setPixmap(std::move(img)); });
    connect(
        pimpl->frameProducerThread.get(),
        &FrameProducerThread::logMessage,
        this,
        [this](QString s) { pimpl->ui.plainTextEdit->appendPlainText(std::move(s)); });
    pimpl->frameProducerThread->start();
}

MainWindow::~MainWindow()
{
    pimpl->frameProducerThread->wait();
}
