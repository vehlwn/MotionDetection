#include "PixmapScene.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QList>
#include <QTransform>
#include <algorithm>

PixmapScene::PixmapScene(QObject* parent)
    : base{parent}
{
}

void PixmapScene::makeItemsControllable(const bool b)
{
    for(auto item : items())
    {
        item->setFlag(QGraphicsItem::ItemIsSelectable, b);
        item->setFlag(QGraphicsItem::ItemIsMovable, b);
        item->setFlag(QGraphicsItem::ItemIsFocusable, b);
    }
}

void PixmapScene::resetZvalues()
{
    int v = 0;
    for(auto item : items())
    {
        item->setZValue(static_cast<qreal>(v));
        v++;
    }
}

void PixmapScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if(const auto clickedItem = itemAt(event->scenePos(), QTransform{}))
    {
        const qreal clickedValue = clickedItem->zValue();
        const auto v = items();
        const qreal maxValue = v.front()->zValue();
        // Decrease each value that is greater than clickedValue.
        for(auto item : v)
        {
            const qreal itemValue = item->zValue();
            if(itemValue > clickedValue)
                item->setZValue(itemValue - 1);
        }
        // Set clickedItem.value = maxValue;
        clickedItem->setZValue(maxValue);
    }
    base::mousePressEvent(event);
}
