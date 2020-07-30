#include "utils.h"

#include <FrameProducerThread.h>
#include <QImage>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

struct FrameProducerThread::Impl
{
    cv::VideoCapture cap;
};

FrameProducerThread::FrameProducerThread(QObject* parent)
    : base{parent}
    , pimpl{std::make_unique<Impl>()}
{
}

void FrameProducerThread::run()
{
    const std::string fname =
        R"(K:\Road traffic video for object recognition.avi)";
    if(!pimpl->cap.open(fname))
    {
        emit logMessage(
            QString{"Failed to open '%1'"}.arg(QString::fromStdString(fname)));
        return;
    }
    emit logMessage(QString{"CAP_PROP_FRAME_WIDTH = %1"}.arg(
        pimpl->cap.get(cv::CAP_PROP_FRAME_WIDTH)));

    /* emit newFrame(utils::cvMat2QPixmap(mat)); */
}
