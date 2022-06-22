#include "utils.h"

#include <QDebug>
#include <QTextStream>
#include <array>
#include <cstdint>
#include <numeric>

namespace {
auto fourcc2array(int value)
{
    std::array<std::uint8_t, 4> ret{};
    ret[0] = (std::uint8_t)(value & 0XFF);
    ret[1] = (std::uint8_t)((value & 0XFF00) >> 8);
    ret[2] = (std::uint8_t)((value & 0XFF0000) >> 16);
    ret[3] = (std::uint8_t)((value & 0XFF000000) >> 24);
    return ret;
}
} // namespace

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

QString fourcc2string(int value)
{
    const auto arr = fourcc2array(value);
    return std::accumulate(arr.begin(), arr.end(), QString{});
}

QString fourcc2hexString(int value)
{
    QString ret;
    QTextStream in{&ret};
    in.setIntegerBase(16);
    in.setPadChar('0');
    in.setFieldWidth(6);
    in.setNumberFlags(QTextStream::ShowBase);
    in << value;
    return ret;
}
} // namespace utils
