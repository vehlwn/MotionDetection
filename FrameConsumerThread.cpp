#include "FrameConsumerThread.h"

#include <atomic>

struct FrameConsumerThread::Impl
{
    std::atomic_bool stopped = false;
    std::weak_ptr<BufferedVideoReader::DataQue> queue;
    double fps = 25;
};

FrameConsumerThread::FrameConsumerThread(
    QObject* parent,
    std::weak_ptr<BufferedVideoReader::DataQue> queue)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->queue = std::move(queue);
}
FrameConsumerThread::~FrameConsumerThread()
{
    stop();
}

void FrameConsumerThread::run()
{
    pimpl->stopped = false;
    while(!pimpl->stopped)
    {
        if(auto p = pimpl->queue.lock())
        {
            emit logMessage(QString{"que size = %1"}.arg(p->size()));
            if(auto optData = p->waitPop())
            {
                emit newData(std::move(*optData));
                /* QThread::msleep(33); */
                continue;
            }
        }
        break;
    }
}

void FrameConsumerThread::stop()
{
    pimpl->stopped = true;
}
