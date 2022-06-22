#include "utils.h"

#include <QDebug>

namespace utils {
QImage cvMat2QImage(const cv::Mat& mat)
{
    if(mat.empty())
        return {};
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
    QImage ret{
        reinterpret_cast<const uchar*>(mat.data),
        mat.cols,
        mat.rows,
        static_cast<int>(mat.step),
        format};
    return ret.copy();
}

cv::Mat QImage2cvMat(const QImage& img)
{
    if(img.isNull())
        return {};
    int type{};
    switch(const auto format = img.format())
    {
    case QImage::Format_Grayscale8:
        type = CV_8UC1;
        break;
    case QImage::Format_BGR888:
        type = CV_8UC3;
        break;
    default:
        qDebug() << "Unknown img format:" << format;
        return {};
    }
    cv::Mat ret{
        img.height(),
        img.width(),
        type,
        const_cast<void*>(static_cast<const void*>(img.constBits())),
        static_cast<std::size_t>(img.bytesPerLine())};
    return ret.clone();
}
} // namespace utils
