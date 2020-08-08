#include "FrameConsumerThread.h"

#include "PainterUtils.h"
#include "utils.h"

#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <atomic>
#include <memory>
#include <opencv2/videoio.hpp>

namespace {
QString getDatetimeFilename()
{
    auto now = QDateTime::currentDateTime();
    return now.toString("yyyy-MM-dd hh.mm.ss");
}
} // namespace

struct FrameConsumerThread::Impl
{
    std::weak_ptr<BufferedVideoReader::DataQue> queue;
    std::unique_ptr<FrameConsumerWorker> timerWorker;
    VideoWriterOptions videoOptions;
    cv::VideoWriter out;
};

FrameConsumerThread::FrameConsumerThread(QObject* parent)
    : base{parent}
    , pimpl{std::make_shared<Impl>()}
{
}

FrameConsumerThread::~FrameConsumerThread() = default;

void FrameConsumerThread::setOptions(
    std::weak_ptr<BufferedVideoReader::DataQue> queue,
    VideoWriterOptions videoOptions)
{
    pimpl->queue = std::move(queue);
    pimpl->videoOptions = std::move(videoOptions);
    if(!pimpl->videoOptions.outputDir.mkpath("."))
    {
        emit logMessage(QString{"FrameConsumerThread failed to create dir '%1'"}.arg(
            pimpl->videoOptions.outputDir.path()));
        return;
    }
    emit logMessage(QString{"FrameConsumerThread created dir '%1'"}.arg(
        pimpl->videoOptions.outputDir.path()));

    const QString outFname = pimpl->videoOptions.outputDir.absoluteFilePath(
        getDatetimeFilename() + pimpl->videoOptions.outputExtension);
    if(!pimpl->out.open(
           outFname.toStdString(),
           pimpl->videoOptions.fourcc,
           pimpl->videoOptions.fps,
           {pimpl->videoOptions.width, pimpl->videoOptions.height}))
    {
        emit logMessage(
            QString{"FrameConsumerThread failed to open output vieo file '%1'"}.arg(
                outFname));
        return;
    }
    emit logMessage(QString{"FrameConsumerThread opened file '%1'"}.arg(outFname));

    pimpl->timerWorker = std::make_unique<FrameConsumerWorker>(
        1. / pimpl->videoOptions.fps * 1000.,
        Qt::PreciseTimer,
        pimpl);
    connect(
        pimpl->timerWorker.get(),
        &FrameConsumerWorker::newData,
        this,
        &FrameConsumerThread::newData);

    pimpl->timerWorker->moveToThread(this);
}

void FrameConsumerThread::run()
{
    exec();
}

void FrameConsumerThread::stop()
{
    quit();
}

FrameConsumerWorker::FrameConsumerWorker(
    int msec,
    Qt::TimerType atype,
    std::weak_ptr<FrameConsumerThread::Impl> pimpl)
    : base{msec, atype}
    , pimpl{std::move(pimpl)}
{
}

FrameConsumerWorker::~FrameConsumerWorker() = default;

void FrameConsumerWorker::onTimeout()
{
    std::shared_ptr<FrameConsumerThread::Impl> pimpl = this->pimpl.lock();
    if(!pimpl)
        return;
    if(auto que = pimpl->queue.lock())
    {
        if(auto img = que->waitPop())
        {
            painterUtils::drawDatetime(img->frame, 0, 0);
            painterUtils::drawTextWithBackground(
                img->frame,
                QString{"moving=%1"}.arg(img->movingArea),
                150,
                0);
            painterUtils::drawTextWithBackground(
                img->frame,
                QString{"que=%1"}.arg(que->size()),
                250,
                0);
            painterUtils::drawRecordingCircle(img->frame, 10, 20);
            pimpl->out.write(utils::QImage2cvMat(img->frame));
            emit newData(std::move(*img));
        }
    }
}
