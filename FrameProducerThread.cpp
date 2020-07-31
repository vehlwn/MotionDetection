#include "FrameProducerThread.h"

#include "utils.h"

#include <QImage>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

struct FrameProducerThread::Impl
{
    cv::VideoCapture cap;
    bool stopped = false;
};

FrameProducerThread::FrameProducerThread(QObject* parent)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
}

void FrameProducerThread::run()
{
    emit logMessage(QString::fromStdString(cv::getBuildInformation()));
    const std::string fname = R"(K:\60-BPM-Metronome.mp4)";
    if(!pimpl->cap.open(fname))
    {
        emit logMessage(
            QString{"Failed to open '%1'"}.arg(QString::fromStdString(fname)));
        return;
    }
    emit logMessage(QString{"CAP_PROP_FRAME_WIDTH = %1"}.arg(
        pimpl->cap.get(cv::CAP_PROP_FRAME_WIDTH)));
    emit logMessage(QString{"CAP_PROP_FRAME_HEIGHT = %1"}.arg(
        pimpl->cap.get(cv::CAP_PROP_FRAME_HEIGHT)));
    emit logMessage(QString{"CAP_PROP_FPS = %1"}.arg(pimpl->cap.get(cv::CAP_PROP_FPS)));
    emit logMessage(QString{"CAP_PROP_FRAME_COUNT = %1"}.arg(
        pimpl->cap.get(cv::CAP_PROP_FRAME_COUNT)));
    emit logMessage(
        QString{"CAP_PROP_BITRATE = %1"}.arg(pimpl->cap.get(cv::CAP_PROP_BITRATE)));

    const auto minFamePeriod = std::chrono::microseconds{
        static_cast<int>(1.0 / pimpl->cap.get(cv::CAP_PROP_FPS) * 1.0e6)};
    emit logMessage(QString{"minFamePeriod = %1"}.arg(minFamePeriod.count()));
    while(!pimpl->stopped)
    {
        cv::Mat frame;
        const bool ret = pimpl->cap.read(frame);
        if(!ret)
            break;

        emit newFrame(utils::cvMat2QPixmap(frame));

        QThread::usleep(minFamePeriod.count());
    }
}

void FrameProducerThread::stopStreaming()
{
    pimpl->stopped = true;
}
