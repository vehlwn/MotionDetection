#include "FrameWriterThread.h"

#include "utils.h"

#include <QDateTime>
#include <opencv2/videoio.hpp>

namespace {
QString getDatetimeFilename()
{
    auto now = QDateTime::currentDateTime();
    return now.toString("yyyy-MM-dd hh.mm");
}
} // namespace

struct FrameWriterThread::Impl
{
    std::unique_ptr<FrameWriterWorker> writerWorker;
    QDir outputDir;
    QString outputExtension;
    cv::VideoWriter out;
};

FrameWriterThread::FrameWriterThread(
    QObject* parent,
    QDir outputDir,
    QString outputExtension,
    int fourcc,
    double fps,
    int width,
    int height)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->outputDir = std::move(outputDir);
    if(!pimpl->outputDir.mkpath("."))
    {
        emit logMessage(QString{"FrameWriterThread failed to create dir '%1'"}.arg(
            pimpl->outputDir.path()));
        return;
    }
    pimpl->writerWorker =
        std::unique_ptr<FrameWriterWorker>(new FrameWriterWorker{this});
    const QString outFname =
        pimpl->outputDir.absoluteFilePath(getDatetimeFilename() + outputExtension);
    if(!pimpl->out.open(outFname.toStdString(), fourcc, fps, {width, height}))
    {
        emit logMessage(
            QString{"Failed to open output vieo file '%1'"}.arg(outFname));
        return;
    }
    emit logMessage(QString{"Writer opened file '%1'"}.arg(outFname));
    connect(
        this,
        &FrameWriterThread::newData,
        pimpl->writerWorker.get(),
        &FrameWriterWorker::processData);
    pimpl->writerWorker->moveToThread(this);
}

FrameWriterThread::~FrameWriterThread() = default;

void FrameWriterThread::run()
{
    exec();
}

void FrameWriterThread::stop()
{
    quit();
}

struct FrameWriterWorker::Impl
{
    FrameWriterThread* t{};
};

FrameWriterWorker::FrameWriterWorker(FrameWriterThread* t)
    : pimpl{std::make_unique<Impl>()}
{
    pimpl->t = t;
}

FrameWriterWorker::~FrameWriterWorker() = default;

void FrameWriterWorker::processData(BufferedVideoReader::Data img)
{
    if(!img.frameToWrite.isNull())
        pimpl->t->pimpl->out.write(utils::QPixmap2cvMat(img.frameToWrite));
}
