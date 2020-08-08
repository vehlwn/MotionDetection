#include "BufferedVideoReader.h"

#include "ApplicationSettings.h"
#include "FixedThreadSafeQueue.h"
#include "FrameConsumerThread.h"
#include "FrameProducerThread.h"
#include "VideoWriterOptions.h"

#include <memory>
#include <mutex>
#include <opencv2/videoio.hpp>

struct BufferedVideoReader::Impl
{
    std::shared_ptr<FixedThreadSafeQueue<Data>> frameQue;
    std::shared_ptr<cv::VideoCapture> videoCapture;
    std::unique_ptr<FrameConsumerThread> consumer;
    std::unique_ptr<FrameProducerThread> producer;
};

BufferedVideoReader::BufferedVideoReader()
    : pimpl{std::make_unique<Impl>()}
{
    std::once_flag flag1;
    std::call_once(flag1, [] {
        qRegisterMetaType<Data>("BufferedVideoReader::Data");
    });
}

BufferedVideoReader::~BufferedVideoReader() = default;

void BufferedVideoReader::start()
{
    pimpl->videoCapture = std::make_shared<cv::VideoCapture>();
    const auto& i = ApplicationSettings::i();
    if(i.fileChecked())
    {
        if(!pimpl->videoCapture->open(i.fname().toStdString()))
        {
            emit logMessage(QString{"Failed to open file '%1'"}.arg(i.fname()));
            return;
        }
        emit logMessage(QString{"Opened file '%1'"}.arg(i.fname()));
    }
    else if(i.cameraChecked())
    {
        if(!pimpl->videoCapture->open(i.cameraIndex()))
        {
            emit logMessage(
                QString{"Failed to open camera '%1'"}.arg(i.cameraIndex()));
            return;
        }
        emit logMessage(QString{"Opened camera index %1"}.arg(i.cameraIndex()));
    }
    emit logMessage(QString{"CAP_PROP_FRAME_WIDTH = %1"}.arg(
        pimpl->videoCapture->get(cv::CAP_PROP_FRAME_WIDTH)));
    emit logMessage(QString{"CAP_PROP_FRAME_HEIGHT = %1"}.arg(
        pimpl->videoCapture->get(cv::CAP_PROP_FRAME_HEIGHT)));
    emit logMessage(QString{"CAP_PROP_FPS = %1"}.arg(
        pimpl->videoCapture->get(cv::CAP_PROP_FPS)));
    emit logMessage(QString{"CAP_PROP_FRAME_COUNT = %1"}.arg(
        pimpl->videoCapture->get(cv::CAP_PROP_FRAME_COUNT)));
    emit logMessage(QString{"CAP_PROP_BITRATE = %1"}.arg(
        pimpl->videoCapture->get(cv::CAP_PROP_BITRATE)));

    const double fps = pimpl->videoCapture->get(cv::CAP_PROP_FPS);
    pimpl->frameQue = std::make_shared<FixedThreadSafeQueue<Data>>(
        static_cast<std::size_t>(i.frameBufferSize()));
    pimpl->producer = std::make_unique<FrameProducerThread>(
        this,
        pimpl->frameQue,
        pimpl->videoCapture);
    VideoWriterOptions videoOptions;
    videoOptions.outputDir.setPath(i.outputFolder());
    videoOptions.outputExtension = ".mkv";
    videoOptions.fourcc =
        static_cast<int>(pimpl->videoCapture->get(cv::CAP_PROP_FOURCC));
    videoOptions.fps = fps;
    videoOptions.width =
        static_cast<int>(pimpl->videoCapture->get(cv::CAP_PROP_FRAME_WIDTH));
    videoOptions.height =
        static_cast<int>(pimpl->videoCapture->get(cv::CAP_PROP_FRAME_HEIGHT));
    pimpl->consumer =
        std::make_unique<FrameConsumerThread>(this, pimpl->frameQue, videoOptions);
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
    pimpl->producer->start();
    pimpl->consumer->start();
    pimpl->frameQue->start();
}

void BufferedVideoReader::stop()
{
    if(pimpl->producer)
        pimpl->producer->stop();
    if(pimpl->consumer)
        pimpl->consumer->stop();
    if(pimpl->frameQue)
        pimpl->frameQue->stop();
}

void BufferedVideoReader::wait()
{
    if(pimpl->producer)
        pimpl->producer->wait();
    if(pimpl->consumer)
        pimpl->consumer->wait();
}

void BufferedVideoReader::waitStop()
{
    stop();
    wait();
}
