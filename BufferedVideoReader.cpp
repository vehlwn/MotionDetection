#include "BufferedVideoReader.h"

#include "FixedThreadSafeQueue.h"
#include "FrameConsumerThread.h"
#include "FrameProducerThread.h"

#include <mutex>

struct BufferedVideoReader::Impl
{
    std::shared_ptr<FixedThreadSafeQueue<Data>> frameQue;
    std::shared_ptr<FrameProducerThread> producer;
    std::shared_ptr<FrameConsumerThread> consumer;
};

BufferedVideoReader::BufferedVideoReader()
    : pimpl{std::make_unique<Impl>()}
{
    {
        std::once_flag flag1;
        std::call_once(flag1, [] {
            qRegisterMetaType<Data>("BufferedVideoReader::Data");
        });
    }
    pimpl->frameQue = std::make_shared<FixedThreadSafeQueue<Data>>();
    pimpl->producer = std::make_shared<FrameProducerThread>(this, pimpl->frameQue);
    pimpl->consumer = std::make_shared<FrameConsumerThread>(this, pimpl->frameQue);
    connect(
        pimpl->consumer.get(),
        &FrameConsumerThread::newData,
        this,
        &BufferedVideoReader::newData);
    connect(
        pimpl->consumer.get(),
        &FrameConsumerThread::logMessage,
        this,
        &BufferedVideoReader::logMessage);
    connect(
        pimpl->producer.get(),
        &FrameProducerThread::logMessage,
        this,
        &BufferedVideoReader::logMessage);
}

BufferedVideoReader::~BufferedVideoReader()
{
    stop();
}

void BufferedVideoReader::start()
{
    waitStop();
    pimpl->producer->start();
    pimpl->consumer->start();
    pimpl->frameQue->start();
}

void BufferedVideoReader::stop()
{
    pimpl->producer->stop();
    pimpl->consumer->stop();
    pimpl->frameQue->stop();
}

void BufferedVideoReader::wait()
{
    pimpl->producer->wait();
    pimpl->consumer->wait();
}

void BufferedVideoReader::waitStop()
{
    stop();
    wait();
}
