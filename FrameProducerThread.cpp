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
    std::weak_ptr<BufferedVideoReader::DataQue> queue,
    std::weak_ptr<cv::VideoCapture> cap)
    : pimpl{std::make_unique<Impl>()}
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
    const auto& i = ApplicationSettings::i();
    const bool gaussianBlurChecked = i.gaussianBlurChecked();
    const int ksize = i.gaussianBlurValue();
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
        if(gaussianBlurChecked)
            cv::GaussianBlur(frame, blurredFrame, {ksize, ksize}, 0);
        else
            blurredFrame = frame;

        pimpl->backSubtractor->apply(blurredFrame, fgmask);

        BufferedVideoReader::Data img;
        img.fgmask = utils::cvMat2QImage(fgmask);
        img.movingArea = cv::countNonZero(fgmask);
        img.frame = utils::cvMat2QImage(frame);
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
