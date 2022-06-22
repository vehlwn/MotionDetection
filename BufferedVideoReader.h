#pragma once

#include "FixedThreadSafeQueue.h"

#include <QObject>
#include <QImage>
#include <memory>

class BufferedVideoReader : public QObject
{
    Q_OBJECT
public:
    struct Data
    {
        QImage frame, fgmask;
        int movingArea{};
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
