#include <graphicactor.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>


GraphicActor::GraphicActor(std::shared_ptr<Common::Actor> actor_ptr, QGraphicsItem* parent) : QGraphicsPixmapItem (parent)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemIsMovable);

    this->actor_ptr = actor_ptr;
    this->parent = parent;
    image = QPixmap(":/Images/boat.png");
    setPixmap(image);
    setScale(0.05);
}

void GraphicActor::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    old_pos = this->scenePos();
    QGraphicsItem::mousePressEvent(event);
}

void GraphicActor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void GraphicActor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    QPointF new_pos = this->scenePos();
    emit actor_is_moved(0, old_pos, new_pos);
    std::cerr << "old pos: " << old_pos.rx() << ", " << old_pos.ry() << "\n";
    std::cerr << "new pos: " << new_pos.rx() << ", " << new_pos.ry() << "\n";
}

