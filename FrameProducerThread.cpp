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
    std::weak_ptr<BufferedVideoReader::DataQue> queue;
};

FrameProducerThread::FrameProducerThread(
    QObject* parent,
    std::weak_ptr<BufferedVideoReader::DataQue> queue)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->queue = std::move(queue);
}

FrameProducerThread::~FrameProducerThread()
{
    stop();
}

void FrameProducerThread::run()
{
    const auto& i = ApplicationSettings::i();
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
            emit logMessage(
                QString{"Failed to open camera '%1'"}.arg(i.cameraIndex()));
            return;
        }
    }
    emit logMessage(QString{"CAP_PROP_FRAME_WIDTH = %1"}.arg(
        pimpl->cap.get(cv::CAP_PROP_FRAME_WIDTH)));
    emit logMessage(QString{"CAP_PROP_FRAME_HEIGHT = %1"}.arg(
        pimpl->cap.get(cv::CAP_PROP_FRAME_HEIGHT)));
    emit logMessage(
        QString{"CAP_PROP_FPS = %1"}.arg(pimpl->cap.get(cv::CAP_PROP_FPS)));
    emit logMessage(QString{"CAP_PROP_FRAME_COUNT = %1"}.arg(
        pimpl->cap.get(cv::CAP_PROP_FRAME_COUNT)));
    emit logMessage(
        QString{"CAP_PROP_BITRATE = %1"}.arg(pimpl->cap.get(cv::CAP_PROP_BITRATE)));

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
        /* cv::Mat blurredFrame; */
        /* cv::GaussianBlur(frame, blurredFrame, {21, 21}, 0); */
        /* backSubtractor->apply(blurredFrame, fgmask); */

        BufferedVideoReader::Data img;
        img.fgmask = utils::cvMat2QPixmap(fgmask);
        img.frame = utils::cvMat2QPixmap(frame);
        if(auto p = pimpl->queue.lock())
            p->waitPush(std::move(img));
        else
            break;
    }
}

void FrameProducerThread::stop()
{
    pimpl->stopped = true;
}
