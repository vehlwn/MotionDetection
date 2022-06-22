#include "MainWindow.h"

#include "FrameProducerThread.h"
#include "PixmapScene.h"
#include "ui_MainWindow.h"

#include <QAbstractButton>
#include <QDebug>
#include <QFileDialog>
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
    FrameProducerThread frameProducerThread;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->ui.setupUi(this);

    pimpl->scene = new PixmapScene;
    pimpl->frameItem = new QGraphicsPixmapItem;
    pimpl->fgmaskItem = new QGraphicsPixmapItem;

    pimpl->ui.graphicsView->setScene(pimpl->scene);
    pimpl->scene->addItem(pimpl->frameItem);
    pimpl->scene->addItem(pimpl->fgmaskItem);
    pimpl->scene->makeItemsControllable(true);
    pimpl->scene->resetZvalues();
    connect(
        &pimpl->frameProducerThread,
        &FrameProducerThread::newFrame,
        this,
        [this](QPixmap img) { pimpl->frameItem->setPixmap(img); });
    connect(
        &pimpl->frameProducerThread,
        &FrameProducerThread::newFgmask,
        this,
        [this](QPixmap img) { pimpl->fgmaskItem->setPixmap(img); });
    connect(
        &pimpl->frameProducerThread,
        &FrameProducerThread::logMessage,
        this,
        [this](QString s) { pimpl->ui.plainTextEdit->appendPlainText(std::move(s)); });
    pimpl->frameProducerThread.start();

    connectUiSlots();
    pimpl->ui.radioButtonCamera->setChecked(true);
    pimpl->ui.lineEditFname->setEnabled(false);
    pimpl->ui.pushButtonOpenFile->setEnabled(false);
}

MainWindow::~MainWindow()
{
    emit pimpl->frameProducerThread.stopStreaming();
    pimpl->frameProducerThread.wait();
}

void MainWindow::connectUiSlots()
{
    connect(pimpl->ui.radioButtonCamera, &QAbstractButton::toggled, this, [this](bool b) {
        pimpl->ui.spinBoxCameraIndex->setEnabled(b);
    });
    connect(pimpl->ui.radioButtonFile, &QAbstractButton::toggled, this, [this](bool b) {
        pimpl->ui.lineEditFname->setEnabled(b);
        pimpl->ui.pushButtonOpenFile->setEnabled(b);
    });
    connect(pimpl->ui.pushButtonOpenFile, &QAbstractButton::clicked, this, [this]() {
        const QString fileName = QFileDialog::getOpenFileName(
            this,
            "Open video file",
            "",
            "AVI files (*.avi);;MP4 files (*.mp4);;All files (*.*)");

        if(fileName.isEmpty())
            return;
        pimpl->ui.lineEditFname->setText(fileName);
    });
}
