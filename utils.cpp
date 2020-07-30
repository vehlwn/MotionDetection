#include "utils.h"

namespace utils {
QPixmap cvMat2QPixmap(const cv::Mat& mat)
{
    return QPixmap::fromImage(QImage{
        reinterpret_cast<const uchar*>(mat.data),
        mat.cols,
        mat.rows,
        static_cast<int>(mat.step),
        QImage::Format_BGR888});
}
} // namespace utils
