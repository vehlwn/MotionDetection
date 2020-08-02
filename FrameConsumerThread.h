#pragma once

#include "BufferedVideoReader.h"

#include <QObject>
#include <QPixmap>
#include <QThread>
#include <memory>

class FrameConsumerThread : public QThread
{
    Q_OBJECT
    using base = QThread;

public:
    FrameConsumerThread(
        QObject* parent,
        std::weak_ptr<BufferedVideoReader::DataQue> queue);
    ~FrameConsumerThread();

protected:
    void run() override;

signals:
    void newData(BufferedVideoReader::Data img);
    void logMessage(QString s);

public slots:
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
