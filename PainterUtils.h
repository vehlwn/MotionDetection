#pragma once

#include <QPixmap>

namespace painterUtils {
QPixmap drawDatetime(QPixmap input, int x, int y);
QPixmap drawTextWithBackground(QPixmap input, const QString& text, int x, int y);
QPixmap drawRecordingCircle(QPixmap input, int radius, int padding);
} // namespace painterUtils
