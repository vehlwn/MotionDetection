#include "FrameProducerThread.h"

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
    VideoCaptureOptions videoOptions;
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

void FrameProducerThread::startStreaming(VideoCaptureOptions videoOptions)
{
    stopStreaming();
    pimpl->videoOptions = std::move(videoOptions);
    start();
}

void FrameProducerThread::run()
{
    if(auto fname = std::get_if<QString>(&pimpl->videoOptions.fname))
    {
        if(!pimpl->cap.open(fname->toStdString()))
        {
            emit logMessage(QString{"Failed to open file '%1'"}.arg(*fname));
            return;
        }
    }
    else if(auto index = std::get_if<int>(&pimpl->videoOptions.fname))
    {
        if(!pimpl->cap.open(*index))
        {
            emit logMessage(QString{"Failed to open camera '%1'"}.arg(*index));
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
