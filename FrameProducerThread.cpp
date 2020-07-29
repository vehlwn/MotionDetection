#include <FrameProducerThread.h>
#include <QImage>
#include <opencv2/imgcodecs.hpp>

FrameProducerThread::FrameProducerThread(QObject* parent)
    : base{parent}
{
}

void FrameProducerThread::run()
{
    for(const auto& fname : std::vector<QString>{"a.jpg", "b.jpg"})
    {
        emit logMessage("fname = " + fname);
        const cv::Mat mat = cv::imread(fname.toStdString());
        if(mat.data == nullptr)
        {
            emit logMessage("continue");
            continue;
        }
        /* emit newFrame(QPixmap{fname}); */
        emit newFrame(QPixmap::fromImage(QImage{
            reinterpret_cast<const uchar*>(mat.data),
            mat.cols,
            mat.rows,
            QImage::Format_RGB888}));
        QThread::sleep(2);
    }
}
