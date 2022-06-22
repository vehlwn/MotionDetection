#include <QImage>
#include <opencv2/core/mat.hpp>

namespace utils {
QImage cvMat2QImage(cv::Mat mat);
cv::Mat QImage2cvMat(QImage src);
QString fourcc2string(int value);
QString fourcc2hexString(int value);
} // namespace utils
