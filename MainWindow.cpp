#include "MainWindow.h"

#include "ApplicationSettings.h"
#include "BufferedVideoReader.h"
#include "PixmapScene.h"
#include "PixmapTitleItem.h"
#include "SettingsDialog.h"
#include "ui_MainWindow.h"

#include <QAbstractButton>
#include <QAction>
#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsScene>
#include <memory>

namespace Ui {
class MainWindow;
}

struct MainWindow::Impl
{
    Ui::MainWindow ui;
    PixmapScene* scene{};
    PixmapTitleItem *frameItem{}, *fgmaskItem{};
    BufferedVideoReader videoReader;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->ui.setupUi(this);
    pimpl->ui.splitter->setStretchFactor(0, 5);
    pimpl->ui.splitter->setStretchFactor(1, 1);

    pimpl->scene = new PixmapScene;
    pimpl->frameItem = new PixmapTitleItem;
    pimpl->frameItem->setTitle("Frame");
    pimpl->fgmaskItem = new PixmapTitleItem;
    pimpl->fgmaskItem->setTitle("Fgmask");
    pimpl->fgmaskItem->setPos(100, 100);

    pimpl->ui.graphicsView->setScene(pimpl->scene);
    pimpl->scene->addItem(pimpl->frameItem);
    pimpl->scene->addItem(pimpl->fgmaskItem);
    pimpl->scene->makeItemsControllable(true);
    pimpl->scene->resetZvalues();
    connect(
        &pimpl->videoReader,
        &BufferedVideoReader::newData,
        this,
        [this](BufferedVideoReader::Data img) {
            pimpl->frameItem->setPixmap(QPixmap::fromImage(img.frame));
            pimpl->fgmaskItem->setPixmap(QPixmap::fromImage(img.fgmask));
        });
    connect(
        &pimpl->videoReader,
        &BufferedVideoReader::logMessage,
        this,
        &MainWindow::logMessage);
    connect(
        pimpl->ui.pushButtonStart,
        &QAbstractButton::clicked,
        &pimpl->videoReader,
        [this] {
            pimpl->videoReader.waitStop();
            pimpl->videoReader.start();
        });
    connect(
        pimpl->ui.actionQuit,
        &QAction::triggered,
        this,
        &QCoreApplication::quit);
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
    pimpl->videoReader.waitStop();
}

void MainWindow::logMessage(QString s)
{
    pimpl->ui.plainTextEdit->appendPlainText(std::move(s));
}
