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
        std::weak_ptr<BufferedVideoReader::DataQue> queue,
        std::weak_ptr<cv::VideoCapture> cap);
    ~FrameProducerThread();

protected:
    void run() override;

signals:
    void logMessage(QString s);
    void ranOutOfFrames();

public slots:
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
