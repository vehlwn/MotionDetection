#pragma once

#include <QPaintDevice>

namespace painterUtils {
void drawDatetime(QPaintDevice& img, int x, int y);
void drawTextWithBackground(QPaintDevice& img, const QString& text, int x, int y);
void drawRecordingCircle(QPaintDevice& img, int radius, int padding);
} // namespace painterUtils
