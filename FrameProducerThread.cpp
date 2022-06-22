#include "FrameProducerThread.h"

#include "ApplicationSettings.h"
#include "utils.h"

#include <QDebug>
#include <QImage>
#include <atomic>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/videoio.hpp>

struct FrameProducerThread::Impl
{
    cv::VideoCapture cap;
    std::atomic_bool stopped = false;
};

FrameProducerThread::FrameProducerThread(QObject* parent)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
}

FrameProducerThread::~FrameProducerThread()
{
    stopStreaming();
}

void FrameProducerThread::startStreaming()
{
    stopStreaming();
    start();
}

void FrameProducerThread::run()
{
    auto& i = ApplicationSettings::i();
    if(i.fileChecked())
    {
        if(!pimpl->cap.open(i.fname().toStdString()))
        {
            emit logMessage(QString{"Failed to open file '%1'"}.arg(i.fname()));
            return;
        }
    }
    else if(i.cameraChecked())
    {
        if(!pimpl->cap.open(i.cameraIndex()))
        {
            emit logMessage(QString{"Failed to open camera '%1'"}.arg(i.cameraIndex()));
            return;
        }
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

    const int history = 100;
    const double varThreshold = 16;
    const bool detectShadows = false;
    auto backSubtractor =
        cv::createBackgroundSubtractorMOG2(history, varThreshold, detectShadows);
    pimpl->stopped = false;
    while(!pimpl->stopped)
    {
        cv::Mat frame;
        const bool ret = pimpl->cap.read(frame);
        if(!ret)
            break;

        cv::Mat fgmask;
        cv::Mat blurredFrame;
        cv::GaussianBlur(frame, blurredFrame, {21, 21}, 0);
        backSubtractor->apply(blurredFrame, fgmask);
        emit newFrame(utils::cvMat2QPixmap(frame));
        emit newFgmask(utils::cvMat2QPixmap(fgmask));

        QThread::usleep(minFamePeriod.count());
    }
}

void FrameProducerThread::stopStreaming()
{
    pimpl->stopped = true;
    wait();
}
