#include "PixmapTitleItem.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsScene>
#include <QLinearGradient>
#include <QPainter>
#include <QPixmap>
#include <QPoint>

namespace {
const QColor TITLE_TEXT_COLOR = Qt::white;
const QColor TITLE_BAR_SELECTED_COLOR = Qt::darkBlue;
const QColor TITLE_BAR_UNSELECTED_COLOR = Qt::darkGray;
constexpr int TITLE_TEXT_PADDING = 3;
} // namespace

struct PixmapTitleItem::Impl
{
    QPixmap pixmap;
    QString titleText;
    int titleTextHeight{};
    QFont titleFont;
};

PixmapTitleItem::PixmapTitleItem()
    : pimpl{std::make_unique<Impl>()}
{
    setTitle("Title");
}

PixmapTitleItem::~PixmapTitleItem() = default;

QRectF PixmapTitleItem::boundingRect() const
{
    return pimpl->pixmap.rect().adjusted(0, 0, 0, titleBarHeight());
}

void PixmapTitleItem::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* /*option*/,
    QWidget* /*widget*/)
{
    drawTitleBar(painter);
    drawTitleText(painter);
    drawPixmap(painter);
}

void PixmapTitleItem::setPixmap(QPixmap pixmap)
{
    if(pimpl->pixmap.size() != pixmap.size())
        prepareGeometryChange();
    pimpl->pixmap = std::move(pixmap);
    update();
}

void PixmapTitleItem::setTitle(QString s)
{
    pimpl->titleText = std::move(s);
    QFontMetrics fm{pimpl->titleFont};
    pimpl->titleTextHeight = fm.height();
    update();
}

void PixmapTitleItem::drawTitleBar(QPainter* painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    QLinearGradient gradient{titleBarRect().topLeft(), titleBarRect().bottomRight()};
    if(isSelected())
        gradient.setColorAt(0, TITLE_BAR_SELECTED_COLOR);
    else
        gradient.setColorAt(0, TITLE_BAR_UNSELECTED_COLOR);
    gradient.setColorAt(1, Qt::white);
    painter->fillRect(titleBarRect(), QBrush{gradient});
    painter->restore();
}

void PixmapTitleItem::drawTitleText(QPainter* painter)
{
    painter->save();
    painter->setPen(TITLE_TEXT_COLOR);
    painter->drawText(
        titleBarRect().adjusted(
            +TITLE_TEXT_PADDING,
            +TITLE_TEXT_PADDING,
            -TITLE_TEXT_PADDING,
            -TITLE_TEXT_PADDING),
        pimpl->titleText);
    painter->restore();
}

void PixmapTitleItem::drawPixmap(QPainter* painter)
{
    painter->save();
    painter->drawPixmap(0, titleBarHeight(), pimpl->pixmap);
    painter->restore();
}

QRectF PixmapTitleItem::titleBarRect() const
{
    return QRectF{
        0,
        0,
        static_cast<qreal>(pimpl->pixmap.width()),
        static_cast<qreal>(titleBarHeight())};
}

int PixmapTitleItem::titleBarHeight() const
{
    return pimpl->titleTextHeight + TITLE_TEXT_PADDING * 2;
}
