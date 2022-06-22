#include "FrameConsumerThread.h"

#include <QDebug>
#include <QTimer>
#include <atomic>
#include <memory>

struct FrameConsumerThread::Impl
{
    std::weak_ptr<BufferedVideoReader::DataQue> queue;
    std::unique_ptr<FrameConsumerWorker> timerWorker;
};

FrameConsumerThread::FrameConsumerThread(
    QObject* parent,
    std::weak_ptr<BufferedVideoReader::DataQue> queue,
    double fps)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->queue = std::move(queue);
    pimpl->timerWorker = std::unique_ptr<FrameConsumerWorker>(
        new FrameConsumerWorker(this, 1. / fps * 1000., Qt::PreciseTimer));
    connect(
        pimpl->timerWorker.get(),
        &FrameConsumerWorker::newData,
        this,
        &FrameConsumerThread::newData);

    pimpl->timerWorker->moveToThread(this);
    qDebug() << "FrameConsumerThread ctor id:" << QThread::currentThreadId();
}

FrameConsumerThread::~FrameConsumerThread() = default;

void FrameConsumerThread::run()
{
    qDebug() << "FrameConsumerThread run id:" << QThread::currentThreadId();
    exec();
}

void FrameConsumerThread::stop()
{
    quit();
}

struct FrameConsumerWorker::Impl
{
    FrameConsumerThread* t{};
};

FrameConsumerWorker::FrameConsumerWorker(
    FrameConsumerThread* t,
    int msec,
    Qt::TimerType atype)
    : base{msec, atype}
    , pimpl{new Impl}
{
    pimpl->t = t;
}

FrameConsumerWorker::~FrameConsumerWorker() = default;

void FrameConsumerWorker::onTimeout()
{
    /* qDebug() << "FrameConsumerWorker timeout id:" << QThread::currentThreadId();
     */
    if(auto que = pimpl->t->pimpl->queue.lock())
    {
        /* qDebug() << "que size =" << p->size(); */
        if(auto optData = que->waitPop())
        {
            emit newData(std::move(*optData));
        }
    }
}
