#pragma once

#include <QObject>
#include <QPixmap>
#include <QThread>
#include <memory>

class FrameProducerThread : public QThread
{
    Q_OBJECT
    using base = QThread;

public:
    FrameProducerThread(QObject* parent = nullptr);
    ~FrameProducerThread();

protected:
    void run() override;

signals:
    void newFrame(QPixmap img);
    void newFgmask(QPixmap img);
    void logMessage(QString s);

public slots:
    void stopStreaming();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
