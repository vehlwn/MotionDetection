#pragma once

#include <QPixmap>

namespace painterUtils {
void drawDatetime(QPixmap& img, int x, int y);
void drawTextWithBackground(QPixmap& img, const QString& text, int x, int y);
void drawRecordingCircle(QPixmap& img, int radius, int padding);
} // namespace painterUtils
