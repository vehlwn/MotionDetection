#include "MainWindow.h"

#include "ApplicationSettings.h"
#include "FrameProducerThread.h"
#include "PixmapScene.h"
#include "SettingsDialog.h"
#include "ui_MainWindow.h"

#include <QAbstractButton>
#include <QAction>
#include <QCoreApplication>
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
        &MainWindow::logMessage);
    connect(
        pimpl->ui.pushButtonStart,
        &QAbstractButton::clicked,
        &pimpl->frameProducerThread,
        &FrameProducerThread::startStreaming);
    connect(pimpl->ui.actionQuit, &QAction::triggered, this, &QCoreApplication::quit);
    connect(pimpl->ui.actionSettings, &QAction::triggered, this, [] {
        SettingsDialog dialog;
        if(dialog.exec() != QDialog::Accepted)
        {
            return;
        }
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::logMessage(QString s)
{
    pimpl->ui.plainTextEdit->appendPlainText(std::move(s));
}
