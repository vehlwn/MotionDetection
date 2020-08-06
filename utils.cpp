#include "utils.h"

#include <QDebug>
#include <QImage>

namespace utils {
QPixmap cvMat2QPixmap(const cv::Mat& mat)
{
    QImage::Format format;
    switch(mat.type())
    {
    case CV_8UC1:
        format = QImage::Format_Grayscale8;
        break;
    case CV_8UC3:
        format = QImage::Format_BGR888;
        break;
    default:
        qDebug() << "Unknown mat type";
        return {};
    }
    return QPixmap::fromImage(QImage{
        reinterpret_cast<const uchar*>(mat.data),
        mat.cols,
        mat.rows,
        static_cast<int>(mat.step),
        format});
}

cv::Mat QPixmap2cvMat(const QPixmap& src)
{
    auto img = src.toImage().convertToFormat(QImage::Format_BGR888);
    int type = CV_8UC3;
    cv::Mat ret{
        img.height(),
        img.width(),
        type,
        reinterpret_cast<void*>(img.bits()),
        static_cast<std::size_t>(img.bytesPerLine())};
    return ret.clone();
}
} // namespace utils
