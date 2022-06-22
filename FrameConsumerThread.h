#pragma once

#include "BufferedVideoReader.h"
#include "TimerWorker.h"
#include "VideoWriterOptions.h"

#include <QObject>
#include <QPixmap>
#include <QThread>
#include <memory>

class FrameConsumerWorker;
class FileRotationWorker;
class FrameConsumerThread : public QThread
{
    Q_OBJECT
    using base = QThread;

public:
    FrameConsumerThread();
    ~FrameConsumerThread();

    void setOptions(
        std::weak_ptr<BufferedVideoReader::DataQue> queue,
        VideoWriterOptions videoOptions);

protected:
    void run() override;

signals:
    void newData(BufferedVideoReader::Data img);
    void logMessage(QString s);
    void error(QString s);
    void rotateFile();

public slots:
    void stop();

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl;
    friend class FrameConsumerWorker;
    friend class FileRotationWorker;
};

class FrameConsumerWorker : public TimerWorker
{
    Q_OBJECT
    using base = TimerWorker;

public:
    FrameConsumerWorker(
        int msec,
        Qt::TimerType atype,
        std::weak_ptr<FrameConsumerThread::Impl> pimpl);
    ~FrameConsumerWorker();

signals:
    void newData(BufferedVideoReader::Data img);
    void error(QString s);
    void logMessage(QString s);

protected slots:
    void onTimeout() override;

private:
    std::weak_ptr<FrameConsumerThread::Impl> pimpl;
    friend class FrameConsumerThread;
};

class FileRotationWorker : public TimerWorker
{
    Q_OBJECT
    using base = TimerWorker;

public:
    FileRotationWorker(int msec, std::weak_ptr<FrameConsumerThread::Impl> pimpl);
    ~FileRotationWorker();

signals:
    void error(QString s);
    void logMessage(QString s);

protected slots:
    void onTimeout() override;

private:
    std::weak_ptr<FrameConsumerThread::Impl> pimpl;
    friend class FrameConsumerThread;
};
