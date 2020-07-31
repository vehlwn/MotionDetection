#pragma once

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsSceneMouseEvent>

class PixmapScene : public QGraphicsScene
{
    using base = QGraphicsScene;

public:
    explicit PixmapScene(QObject* parent = nullptr);
    void makeItemsControllable(const bool b);
    void resetZvalues();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};
