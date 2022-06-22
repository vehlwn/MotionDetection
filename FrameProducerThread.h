#pragma once

#include "BufferedVideoReader.h"

#include <QObject>
#include <QPixmap>
#include <QThread>
#include <memory>

class FrameProducerThread : public QThread
{
    Q_OBJECT
    using base = QThread;

public:
    FrameProducerThread(QObject* parent,
        std::weak_ptr<BufferedVideoReader::DataQue> queue);
    ~FrameProducerThread();

protected:
    void run() override;

signals:
    void logMessage(QString s);

public slots:
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
