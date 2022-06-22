#include <QImage>
#include <opencv2/core/mat.hpp>

namespace utils {
QImage cvMat2QImage(const cv::Mat& mat);
cv::Mat QImage2cvMat(const QImage& src);
QString fourcc2string(int value);
QString fourcc2hexString(int value);
} // namespace utils
