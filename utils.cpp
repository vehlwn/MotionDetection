#include "utils.h"

#include <QImage>

namespace utils {
QPixmap cvMat2QPixmap(const cv::Mat& mat)
{
    QImage::Format format;
    switch(mat.channels())
    {
    case 1:
        format = QImage::Format_Grayscale8;
        break;
    default:
        format = QImage::Format_BGR888;
        break;
    }
    return QPixmap::fromImage(QImage{
        reinterpret_cast<const uchar*>(mat.data),
        mat.cols,
        mat.rows,
        static_cast<int>(mat.step),
        format});
}
} // namespace utils
