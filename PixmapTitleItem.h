#pragma once

#include <QGraphicsItem>
#include <QPixmap>
#include <memory>

class PixmapTitleItem : public QGraphicsItem
{
    using base = QGraphicsItem;

public:
    PixmapTitleItem();
    ~PixmapTitleItem();

    QRectF boundingRect() const override;
    void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget) override;
    void setPixmap(QPixmap pixmap);
    void setTitle(QString s);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};
