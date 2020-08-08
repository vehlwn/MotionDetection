#include <QImage>
#include <opencv2/core/mat.hpp>

namespace utils {
QImage cvMat2QImage(const cv::Mat& mat);
cv::Mat QImage2cvMat(const QImage& src);
} // namespace utils
