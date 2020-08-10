#pragma once

#include "FixedThreadSafeQueue.h"

#include <QImage>
#include <QObject>
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

private slots:
    void onError(QString s);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
