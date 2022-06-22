#pragma once

#include <QPaintDevice>
#include <QRect>

namespace painterUtils {
QRect drawDatetime(QPaintDevice& img, int x, int y);
QRect drawTextWithBackground(QPaintDevice& img, const QString& text, int x, int y);
void drawRecordingCircle(QPaintDevice& img, int radius, int padding);
} // namespace painterUtils
