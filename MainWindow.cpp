#include "MainWindow.h"

#include "FrameProducerThread.h"
#include "PixmapScene.h"
#include "ui_MainWindow.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

struct MainWindow::Impl
{
    Ui::MainWindow ui;
    PixmapScene* scene{};
    QGraphicsPixmapItem *frameItem{}, *fgmaskItem{};
    std::unique_ptr<FrameProducerThread> frameProducerThread;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->ui.setupUi(this);

    pimpl->scene = new PixmapScene;
    pimpl->frameItem = new QGraphicsPixmapItem;
    pimpl->fgmaskItem = new QGraphicsPixmapItem;

    pimpl->frameProducerThread = std::make_unique<FrameProducerThread>();

    pimpl->ui.graphicsView->setScene(pimpl->scene);
    pimpl->scene->addItem(pimpl->frameItem);
    pimpl->scene->addItem(pimpl->fgmaskItem);
    pimpl->scene->makeItemsControllable(true);
    pimpl->scene->resetZvalues();
    connect(
        pimpl->frameProducerThread.get(),
        &FrameProducerThread::newFrame,
        this,
        [this](QPixmap img) {
            pimpl->frameItem->setPixmap(img);
            pimpl->fgmaskItem->setPixmap(img);
        });
    connect(
        pimpl->frameProducerThread.get(),
        &FrameProducerThread::logMessage,
        this,
        [this](QString s) { pimpl->ui.plainTextEdit->appendPlainText(std::move(s)); });
    pimpl->frameProducerThread->start();
}

MainWindow::~MainWindow()
{
    emit pimpl->frameProducerThread->stopStreaming();
    pimpl->frameProducerThread->wait();
}
