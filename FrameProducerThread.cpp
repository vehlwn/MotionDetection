#include "FrameProducerThread.h"

#include "ApplicationSettings.h"
#include "PainterUtils.h"
#include "utils.h"

#include <QDebug>
#include <QImage>
#include <atomic>
#include <chrono>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/videoio.hpp>

struct FrameProducerThread::Impl
{
    cv::Ptr<cv::BackgroundSubtractorMOG2> backSubtractor;
    std::atomic_bool stopped = false;
    std::weak_ptr<BufferedVideoReader::DataQue> queue;
    std::weak_ptr<cv::VideoCapture> cap;
};

FrameProducerThread::FrameProducerThread(
    QObject* parent,
    std::weak_ptr<BufferedVideoReader::DataQue> queue,
    std::weak_ptr<cv::VideoCapture> cap)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
    pimpl->queue = std::move(queue);
    pimpl->cap = std::move(cap);

    const auto& i = ApplicationSettings::i();
    const int history = i.history();
    const double varThreshold = 16;
    const bool detectShadows = false;
    pimpl->backSubtractor =
        cv::createBackgroundSubtractorMOG2(history, varThreshold, detectShadows);
}

FrameProducerThread::~FrameProducerThread() = default;

void FrameProducerThread::run()
{
    while(!pimpl->stopped)
    {
        cv::Mat frame;
        if(auto cap = pimpl->cap.lock())
        {
            if(!cap->read(frame))
                break;
        }
        else
            break;

        cv::Mat fgmask;
        cv::Mat blurredFrame;
        cv::GaussianBlur(frame, blurredFrame, {21, 21}, 0);
        pimpl->backSubtractor->apply(blurredFrame, fgmask);

        BufferedVideoReader::Data img;
        img.fgmask = utils::cvMat2QPixmap(fgmask);
        cv::Mat thresh;
        cv::threshold(fgmask, thresh, 127, 255, cv::THRESH_BINARY);
        img.movingArea = cv::countNonZero(thresh);
        img.frameToView = utils::cvMat2QPixmap(frame);
        img.frameToView =
            painterUtils::drawDatetime(std::move(img.frameToView), 0, 0);
        img.frameToView = painterUtils::drawTextWithBackground(
            std::move(img.frameToView),
            QString::number(img.movingArea),
            150,
            0);
        img.frameToView =
            painterUtils::drawRecordingCircle(std::move(img.frameToView), 10, 20);
        img.frameToWrite = img.frameToView.copy();
        if(auto que = pimpl->queue.lock())
            que->waitPush(std::move(img));
        else
            break;
    }
}

void FrameProducerThread::stop()
{
    pimpl->stopped = true;
}
