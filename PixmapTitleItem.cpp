#include "PixmapTitleItem.h"

#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPixmap>
#include <QPoint>

namespace {
const QColor TITLE_SELECTED_COLOR = Qt::red;
const QColor TITLE_UNSELECTED_COLOR = Qt::black;
} // namespace

struct PixmapTitleItem::Impl
{
    QPixmap pixmap;
    QString title = "lala";
    int titleAscent = 20;
    QFont titleFont;
};

PixmapTitleItem::PixmapTitleItem()
    : pimpl{std::make_unique<Impl>()}
{
}

PixmapTitleItem::~PixmapTitleItem() = default;

QRectF PixmapTitleItem::boundingRect() const
{
    return pimpl->pixmap.rect().adjusted(0, -20, 0, 0);
}

void PixmapTitleItem::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    painter->save();
    if(isSelected())
        painter->setPen(TITLE_SELECTED_COLOR);
    else
        painter->setPen(TITLE_UNSELECTED_COLOR);
    painter->drawText(0, -20 + pimpl->titleAscent, pimpl->title);
    painter->drawPixmap(0, 0, pimpl->pixmap);
    if(isSelected())
    {
        painter->setBrush(Qt::NoBrush);
        QPen pen;
        pen.setStyle(Qt::DashLine);
        pen.setColor(Qt::black);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }
    painter->restore();
}

void PixmapTitleItem::setPixmap(QPixmap pixmap)
{
    pimpl->pixmap = std::move(pixmap);
    update();
}

void PixmapTitleItem::setTitle(QString s)
{
    pimpl->title = std::move(s);
    QFontMetrics fm{pimpl->titleFont};
    pimpl->titleAscent = fm.ascent();
}
