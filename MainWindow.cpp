#include "MainWindow.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_scene{new QGraphicsScene}
    , m_scenePixmapItem{new QGraphicsPixmapItem}
    , m_frameProducerThread{std::make_unique<FrameProducerThread>()}
{
    ui->setupUi(this);

    ui->graphicsView->setScene(m_scene);
    m_scene->addItem(m_scenePixmapItem);
    connect(
        m_frameProducerThread.get(),
        &FrameProducerThread::newFrame,
        this,
        [this](QPixmap img) {
            m_scenePixmapItem->setPixmap(std::move(img));
        });
    connect(
        m_frameProducerThread.get(),
        &FrameProducerThread::logMessage,
        this,
        [this](QString s) { ui->plainTextEdit->appendPlainText(std::move(s)); });
    m_frameProducerThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    m_frameProducerThread->wait();
}
