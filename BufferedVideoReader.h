#pragma once

#include "FixedThreadSafeQueue.h"

#include <QObject>
#include <QPixmap>
#include <memory>

class BufferedVideoReader : public QObject
{
    Q_OBJECT
public:
    struct Data
    {
        QPixmap frame, fgmask;
    };
    using DataQue = FixedThreadSafeQueue<Data>;

    BufferedVideoReader();
    ~BufferedVideoReader();

signals:
    void newData(Data data);
    void logMessage(QString s);

public slots:
    void start();
    void stop();
    void wait();
    void waitStop();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
