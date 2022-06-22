#pragma once

#include "BufferedVideoReader.h"
#include "TimerWorker.h"

#include <QObject>
#include <QPixmap>
#include <QThread>
#include <memory>

class FrameConsumerWorker;
class FrameConsumerThread : public QThread
{
    Q_OBJECT
    using base = QThread;

public:
    FrameConsumerThread(
        QObject* parent,
        std::weak_ptr<BufferedVideoReader::DataQue> queue,
        double fps);
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
    friend class FrameConsumerWorker;
};

class FrameConsumerWorker : public TimerWorker
{
    Q_OBJECT
        using base = TimerWorker;
public:
    FrameConsumerWorker(FrameConsumerThread* t, int msec, Qt::TimerType atype);
    ~FrameConsumerWorker();

signals:
    void newData(BufferedVideoReader::Data img);

protected slots:
    void onTimeout() override;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl;
};
