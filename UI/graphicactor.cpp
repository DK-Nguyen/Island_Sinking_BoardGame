#include <graphicactor.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>


GraphicActor::GraphicActor(std::shared_ptr<Common::Actor> actor_ptr, QGraphicsItem* parent)
    : QGraphicsPixmapItem (parent)
{

    this->actor_ptr = actor_ptr;
    this->parent = parent;
    actor_type = actor_ptr->getActorType();
    movement_allowed = false;


    if (actor_type.compare("kraken")==0)
    {
        image = QPixmap(":/Images/kraken.png");
    }
    else if (actor_type.compare("seamunster")==0)
    {
        image = QPixmap(":/Images/seamunster.png");
    }
    else if (actor_type.compare("shark")==0)
    {
        image = QPixmap(":/Images/shark.png");
    }
    else
    {
        image = QPixmap(":/Images/vortex.png");
    }

    setPixmap(image);
    setScale(0.08);
}

std::shared_ptr<Common::Actor> GraphicActor::get_actor()
{
    return actor_ptr;
}

std::string GraphicActor::getName()
{
    return "actor";
}

void GraphicActor::allow_movement(bool allowed, std::list<int> actor_id)
{
    for (auto id : actor_id)
    {
        if (id == actor_ptr->getId())
        {
            movement_allowed = allowed;

            if (allowed)
            {
                setFlags(QGraphicsItem::ItemIsSelectable|
                         QGraphicsItem::ItemIsMovable);
            }
            else
            {
                setFlag(QGraphicsItem::ItemIsSelectable, false);
                setFlag(QGraphicsItem::ItemIsMovable, false);
            }

            break;
        }
    }
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
}
