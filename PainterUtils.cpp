#include "PainterUtils.h"

#include <QDateTime>
#include <QFontMetrics>
#include <QPainter>

namespace painterUtils {

QRect drawDatetime(QPaintDevice& img, int x, int y)
{
    const auto now = QDateTime::currentDateTime();
    const QString text = now.toString(Qt::ISODateWithMs);
    return drawTextWithBackground(img, text, x, y);
}

QRect drawTextWithBackground(QPaintDevice& img, const QString& text, int x, int y)
{
    QPainter painter{&img};
    QFont f = painter.font();
    f.setStyleStrategy(QFont::NoAntialias);
    painter.setFont(f);
    QFontMetrics fm{f};
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setBackground(Qt::white);
    painter.setPen(Qt::black);
    // (x, y) is a top left point.
    painter.drawText(x, y + fm.ascent(), text);
    QRect ret = fm.boundingRect(text);
    ret.moveTo(x, y);
    return ret;
}

void drawRecordingCircle(QPaintDevice& img, int radius, int padding)
{
    QPainter painter{&img};
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(
        img.width() - radius * 2 - padding,
        padding,
        radius * 2,
        radius * 2);
}
} // namespace painterUtils
