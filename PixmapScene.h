#pragma once

#include <QGraphicsScene>
#include <QObject>

class PixmapScene : public QGraphicsScene
{
    using base = QGraphicsScene;

public:
    explicit PixmapScene(QObject* parent = nullptr);
};
