#include "FrameConsumerThread.h"

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

FrameConsumerThread::FrameConsumerThread(
    QObject* parent,
    std::weak_ptr<BufferedVideoReader::DataQue> queue,
    VideoWriterOptions videoOptions)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->queue = std::move(queue);
    pimpl->videoOptions = std::move(videoOptions);
    if(!pimpl->videoOptions.outputDir.mkpath("."))
    {
        emit logMessage(QString{"FrameWriterThread failed to create dir '%1'"}.arg(
            pimpl->videoOptions.outputDir.path()));
        return;
    }
    const QString outFname = pimpl->videoOptions.outputDir.absoluteFilePath(
        getDatetimeFilename() + pimpl->videoOptions.outputExtension);
    if(!pimpl->out.open(
           outFname.toStdString(),
           pimpl->videoOptions.fourcc,
           pimpl->videoOptions.fps,
           {pimpl->videoOptions.width, pimpl->videoOptions.height}))
    {
        emit logMessage(
            QString{"Failed to open output vieo file '%1'"}.arg(outFname));
        return;
    }
    emit logMessage(QString{"Writer opened file '%1'"}.arg(outFname));

    pimpl->timerWorker =
        std::unique_ptr<FrameConsumerWorker>(new FrameConsumerWorker(
            this,
            1. / pimpl->videoOptions.fps * 1000.,
            Qt::PreciseTimer));
    connect(
        pimpl->timerWorker.get(),
        &FrameConsumerWorker::newData,
        this,
        &FrameConsumerThread::newData);

    pimpl->timerWorker->moveToThread(this);
}

FrameConsumerThread::~FrameConsumerThread() = default;

void FrameConsumerThread::run()
{
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
    if(auto que = pimpl->t->pimpl->queue.lock())
    {
        /* qDebug() << "que size =" << que->size(); */
        if(auto optData = que->waitPop())
        {
            emit newData(*optData);
            if(!optData->frameToWrite.isNull())
                pimpl->t->pimpl->out.write(
                    utils::QPixmap2cvMat(optData->frameToWrite));
        }
    }
}
