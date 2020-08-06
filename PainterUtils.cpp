#include "PainterUtils.h"

#include <QDateTime>
#include <QFontMetrics>
#include <QPainter>

namespace painterUtils {

QPixmap drawDatetime(QPixmap input, int x, int y)
{
    const auto now = QDateTime::currentDateTime();
    const QString text = now.toString(Qt::ISODateWithMs);
    return drawTextWithBackground(input, text, x, y);
}

QPixmap drawTextWithBackground(QPixmap input, const QString& text, int x, int y)
{
    QPainter painter{&input};
    QFontMetrics fm{painter.font()};
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setBackground(Qt::white);
    painter.setPen(Qt::black);
    // (x, y) is a top left point.
    painter.drawText(x, y + fm.ascent(), text);
    return input;
}

QPixmap drawRecordingCircle(QPixmap input, int radius, int padding)
{
    QPainter painter{&input};
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(
        input.width() - radius * 2 - padding,
        padding,
        radius * 2,
        radius * 2);
    return input;
}
} // namespace painterUtils
