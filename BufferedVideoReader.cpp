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
    if(pimpl->producer && pimpl->producer->isRunning())
        return;
    if(pimpl->consumer && pimpl->consumer->isRunning())
        return;

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
    emit logMessage(QString{"CAP_PROP_FPS = %1"}.arg(
        pimpl->videoCapture->get(cv::CAP_PROP_FPS)));
    emit logMessage(QString{"CAP_PROP_FRAME_COUNT = %1"}.arg(
        pimpl->videoCapture->get(cv::CAP_PROP_FRAME_COUNT)));

    const double fps = pimpl->videoCapture->get(cv::CAP_PROP_FPS);
    pimpl->frameQue = std::make_shared<FixedThreadSafeQueue<Data>>(
        static_cast<std::size_t>(i.frameBufferSize()));
    pimpl->producer =
        std::make_unique<FrameProducerThread>(pimpl->frameQue, pimpl->videoCapture);

    VideoWriterOptions videoOptions;
    videoOptions.outputDir.setPath(i.outputFolder());
    videoOptions.outputExtension = i.outputExtension();
    const std::string fourccStr = i.outputFourCC().toStdString();
    if(fourccStr.size() != 4)
    {
        emit logMessage(QString{"FOURCC must contain 4 characters. Given: '%1'"}.arg(
            i.outputFourCC()));
        return;
    }
    videoOptions.fourcc = cv::VideoWriter::fourcc(
        fourccStr[0],
        fourccStr[1],
        fourccStr[2],
        fourccStr[3]);
    videoOptions.fps = fps;
    int inputWidth = i.recommendedInputWidth();
    int inputHeight = i.recommendedInputHeight();
    if(!pimpl->videoCapture->set(cv::CAP_PROP_FRAME_WIDTH, inputWidth))
        emit logMessage(
            "Warning: capture device does not support custom frame width");
    if(!pimpl->videoCapture->set(cv::CAP_PROP_FRAME_HEIGHT, inputHeight))
        emit logMessage(
            "Warning: capture device does not support custom frame height");
    inputWidth =
        static_cast<int>(pimpl->videoCapture->get(cv::CAP_PROP_FRAME_WIDTH));
    inputHeight =
        static_cast<int>(pimpl->videoCapture->get(cv::CAP_PROP_FRAME_HEIGHT));
    emit logMessage(QString{"Selected input width = %1, height = %2"}
                        .arg(inputWidth)
                        .arg(inputHeight));
    videoOptions.width = inputWidth;
    videoOptions.height = inputHeight;
    pimpl->consumer = std::make_unique<FrameConsumerThread>();
    connect(
        pimpl->consumer.get(),
        &FrameConsumerThread::newData,
        this,
        &BufferedVideoReader::newData);
    connect(
        pimpl->consumer.get(),
        &FrameConsumerThread::error,
        this,
        &BufferedVideoReader::onError);
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
    connect(
        pimpl->producer.get(),
        &FrameProducerThread::ranOutOfFrames,
        this,
        [this] {
            emit logMessage("No more frames available from the source");
            waitStop();
        });
    pimpl->consumer->setOptions(pimpl->frameQue, videoOptions);
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

void BufferedVideoReader::onError(QString s)
{
    waitStop();
    emit logMessage("Error: " + s);
}
