#pragma once

#include "BufferedVideoReader.h"
#include <QDir>
#include <QObject>
#include <QPixmap>
#include <QThread>
#include <memory>

class FrameWriterWorker;
class FrameWriterThread : public QThread
{
    Q_OBJECT
    using base = QThread;

public:
    FrameWriterThread(
        QObject* parent,
        QDir outputDir,
        QString outputExtension,
        int fourcc,
        double fps,
        int width,
        int height);
    ~FrameWriterThread();

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
    friend class FrameWriterWorker;
};

class FrameWriterWorker : public QObject
{
    Q_OBJECT
public:
    ~FrameWriterWorker();

public slots:
    void processData(BufferedVideoReader::Data img);

private:
    FrameWriterWorker(FrameWriterThread* t);
    struct Impl;
    std::unique_ptr<Impl> pimpl;
    friend class FrameWriterThread;
};
