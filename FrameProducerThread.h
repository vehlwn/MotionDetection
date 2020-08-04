#pragma once

#include "BufferedVideoReader.h"

#include <QObject>
#include <QPixmap>
#include <QThread>
#include <memory>
#include <opencv2/videoio.hpp>

class FrameProducerThread : public QThread
{
    Q_OBJECT
    using base = QThread;

public:
    FrameProducerThread(
        QObject* parent,
        std::weak_ptr<BufferedVideoReader::DataQue> queue,
        std::weak_ptr<cv::VideoCapture> cap);
    ~FrameProducerThread();

protected:
    void run() override;

signals:
    void logMessage(QString s);
    void fps(double d);

public slots:
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
