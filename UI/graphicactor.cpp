#include <graphicactor.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>

GraphicActor::GraphicActor(std::shared_ptr<Common::Actor> actor_ptr, QGraphicsItem* parent)
{
    this->actor_ptr = actor_ptr;
    this->parent = parent;
}

GraphicActor::~GraphicActor()
{
    return;
}


