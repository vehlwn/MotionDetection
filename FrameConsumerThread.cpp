#include "FrameConsumerThread.h"

#include "ApplicationSettings.h"
#include "PainterUtils.h"
#include "utils.h"

#include <QDateTime>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <opencv2/videoio.hpp>

struct FrameConsumerThread::Impl
{
    std::weak_ptr<BufferedVideoReader::DataQue> queue;
    std::unique_ptr<FrameConsumerWorker> timerWorker;
    std::unique_ptr<FileRotationWorker> fileRotationWorker;
    VideoWriterOptions videoOptions;
    cv::VideoWriter out;
    int minMovingArea{};
    double deltaWithoutMotion{};
    QDateTime lastMotionPoint = QDateTime::currentDateTime();
    int maxDateTimeTextWidth{}, maxMovingtextWidth{}, maxQueTextWidth{};
};

FrameConsumerThread::FrameConsumerThread()
    : pimpl{std::make_shared<Impl>()}
{
}

FrameConsumerThread::~FrameConsumerThread() = default;

void FrameConsumerThread::setOptions(
    std::weak_ptr<BufferedVideoReader::DataQue> queue,
    VideoWriterOptions videoOptions)
{
    pimpl->queue = std::move(queue);
    pimpl->videoOptions = std::move(videoOptions);
    const auto& i = ApplicationSettings::i();
    pimpl->minMovingArea = i.minMovingArea();
    pimpl->deltaWithoutMotion = i.deltaWithoutMotion();

    if(!pimpl->videoOptions.outputDir.mkpath("."))
    {
        emit error(QString{"Failed to create dir '%1'"}.arg(
            pimpl->videoOptions.outputDir.path()));
        return;
    }
    emit logMessage(
        QString{"Created dir '%1'"}.arg(pimpl->videoOptions.outputDir.path()));

    pimpl->timerWorker = std::make_unique<FrameConsumerWorker>(
        1. / pimpl->videoOptions.fps * 1000.,
        Qt::PreciseTimer,
        pimpl);
    connect(
        pimpl->timerWorker.get(),
        &FrameConsumerWorker::newData,
        this,
        &FrameConsumerThread::newData);
    connect(
        pimpl->timerWorker.get(),
        &FrameConsumerWorker::error,
        this,
        [this](QString s) {
            stop();
            emit error(s);
        });
    connect(
        pimpl->timerWorker.get(),
        &FrameConsumerWorker::logMessage,
        this,
        &FrameConsumerThread::logMessage);
    pimpl->timerWorker->moveToThread(this);

    const auto period = static_cast<int>(std::round(i.fileRotationMsec()));
    pimpl->fileRotationWorker =
        std::make_unique<FileRotationWorker>(period, Qt::PreciseTimer, pimpl);
    connect(
        pimpl->fileRotationWorker.get(),
        &FileRotationWorker::logMessage,
        this,
        &FrameConsumerThread::logMessage);
    connect(
        pimpl->fileRotationWorker.get(),
        &FileRotationWorker::error,
        this,
        [this](QString s) {
            stop();
            emit error(s);
        });
    connect(
        this,
        &FrameConsumerThread::rotateFile,
        pimpl->fileRotationWorker.get(),
        &FileRotationWorker::onTimeout);
    pimpl->fileRotationWorker->moveToThread(this);

    emit rotateFile();
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
    auto pimpl = this->pimpl.lock();
    if(!pimpl)
        return;
    auto que = pimpl->queue.lock();
    if(!que)
        return;
    auto img = que->waitPop();
    if(!img)
        return;
    if(!pimpl->out.isOpened())
        return;
    constexpr int TEXT_INTERVAL = 20;
    int textX = 0;
    QRect textRect = painterUtils::drawDatetime(img->frame, textX, 0);
    pimpl->maxDateTimeTextWidth =
        std::max(pimpl->maxDateTimeTextWidth, textRect.width());
    textX += pimpl->maxDateTimeTextWidth + TEXT_INTERVAL;
    textRect = painterUtils::drawTextWithBackground(
        img->frame,
        QString{"moving=%1"}.arg(img->movingArea),
        textX,
        0);
    pimpl->maxMovingtextWidth =
        std::max(pimpl->maxMovingtextWidth, textRect.width());
    textX += pimpl->maxMovingtextWidth + TEXT_INTERVAL;
    textRect = painterUtils::drawTextWithBackground(
        img->frame,
        QString{"que=%1"}.arg(que->size()),
        textX,
        0);
    pimpl->maxQueTextWidth = std::max(pimpl->maxQueTextWidth, textRect.width());

    const auto now = QDateTime::currentDateTime();
    bool recording = true;
    if(img->movingArea >= pimpl->minMovingArea)
        pimpl->lastMotionPoint = now;
    else if(pimpl->lastMotionPoint.msecsTo(now) / 1000. >= pimpl->deltaWithoutMotion)
        recording = false;
    if(recording)
    {
        pimpl->out.write(utils::QImage2cvMat(img->frame));
        painterUtils::drawRecordingCircle(img->frame, 10, 20);
    }
    emit newData(std::move(*img));
}

FileRotationWorker::FileRotationWorker(
    int msec,
    Qt::TimerType atype,
    std::weak_ptr<FrameConsumerThread::Impl> pimpl)
    : base{msec, atype}
    , pimpl{std::move(pimpl)}
{
}

FileRotationWorker::~FileRotationWorker() = default;

void FileRotationWorker::onTimeout()
{
    auto pimpl = this->pimpl.lock();
    if(!pimpl)
        return;
    const auto now = QDateTime::currentDateTime();
    const QString dateComponent = now.toString("yyyy-MM-dd");
    const QString timeComponent = now.toString("hh.mm.ss");
    QDir dateDir = pimpl->videoOptions.outputDir;
    dateDir.mkpath(dateComponent);
    dateDir.cd(dateComponent);
    const QString outFname = dateDir.absoluteFilePath(
        timeComponent + pimpl->videoOptions.outputExtension);
    if(!pimpl->out.open(
           outFname.toStdString(),
           pimpl->videoOptions.fourcc,
           pimpl->videoOptions.fps,
           {pimpl->videoOptions.width, pimpl->videoOptions.height}))
    {
        const int fourcc = pimpl->videoOptions.fourcc;
        emit error(QString{
            "Failed to open output video file '%1'. Unsupported FOURCC %2/'%3'?"}
                       .arg(outFname)
                       .arg(utils::fourcc2hexString(fourcc))
                       .arg(utils::fourcc2string(fourcc)));
        return;
    }
    emit logMessage(QString{"Opened file '%1'"}.arg(outFname));
}
