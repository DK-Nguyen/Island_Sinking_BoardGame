#include <graphictransport.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>


GraphicTransport::GraphicTransport(std::shared_ptr<Common::Transport> transport_ptr, QGraphicsItem* parent)
    : QGraphicsPixmapItem (parent)
{

    this->transport_ptr = transport_ptr;
    this->parent = parent;
    this->movement_allowed = false;

    auto transport_type = transport_ptr->getTransportType();

    if (transport_type.compare("boat")==0)
    {
        image = QPixmap(":/Images/boat.png");
    }
    else
    {
        image = QPixmap(":/Images/dolphin.png");
    }

    setPixmap(image);
    setScale(0.08);
}

std::shared_ptr<Common::Transport> GraphicTransport::get_transport()
{
    return transport_ptr;
}

std::string GraphicTransport::getName()
{
    return "transport";
}

void GraphicTransport::allow_movement(bool allowed)
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
}


void GraphicTransport::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    old_pos = this->scenePos();
    QGraphicsItem::mousePressEvent(event);
}

void GraphicTransport::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void GraphicTransport::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    QPointF new_pos = this->scenePos();
    emit transport_is_moved(0, old_pos, new_pos);
}

void GraphicTransport::dropEvent(QGraphicsSceneDragDropEvent *event)
{
     event->setAccepted(true);
     update();
}

void GraphicTransport::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(true);
    update();
}

void GraphicTransport::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    update();
}
