#pragma once

#include "qobject.h"

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

protected:
    void run() override;

signals:
    void newFrame(QPixmap img);
    void logMessage(QString s);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
