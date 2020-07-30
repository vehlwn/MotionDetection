#include <QPixmap>
#include <opencv2/core/mat.hpp>

namespace utils {
QPixmap cvMat2QPixmap(const cv::Mat& mat);
}
