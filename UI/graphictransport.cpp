#include <graphictransport.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>


GraphicTransport::GraphicTransport(std::shared_ptr<Common::Transport> transport_ptr, double scale, QGraphicsItem* parent)
    : QGraphicsPolygonItem(parent)
{

    this->transport_ptr = transport_ptr;
    this->parent = parent;
    this->movement_allowed = false;

    auto transport_type = transport_ptr->getTransportType();
    QVector<QPointF> vertices;

    if (transport_type.compare("boat")==0)
    {

        vertices << QPointF(0.5, 4.0)*scale;
        vertices << QPointF(2.9, 4.0)*scale;
        vertices << QPointF(2.9, 2.5)*scale;
        vertices << QPointF(2.0, 3.25)*scale;
        vertices << QPointF(2.9, 4.0)*scale;
        vertices << QPointF(3.1, 4.0)*scale;
        vertices << QPointF(3.1, 2.5)*scale;
        vertices << QPointF(3.5, 3.25)*scale;
        vertices << QPointF(3.1, 4.0)*scale;
        vertices << QPointF(5.5, 4.0)*scale;
        vertices << QPointF(5.5, 4.5)*scale;
        vertices << QPointF(3.0, 6.5)*scale;
        vertices << QPointF(0.5, 4.5)*scale;
    }
    else
    {
        vertices << QPointF(1.5, 4.0)*scale;
        vertices << QPointF(2.0, 4.75)*scale;
        vertices << QPointF(2.5, 4.0)*scale;
        vertices << QPointF(4.0, 4.0)*scale;
        vertices << QPointF(4.5, 4.5)*scale;
        vertices << QPointF(4.0, 5.5)*scale;
        vertices << QPointF(2.5, 5.5)*scale;
        vertices << QPointF(2.0, 4.75)*scale;
        vertices << QPointF(1.5, 5.5)*scale;

    }

    QPolygonF poly(vertices);
    setPolygon(poly);
    setAcceptDrops(true);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::black);
    setBrush(brush);
}

std::shared_ptr<Common::Transport> GraphicTransport::get_transport()
{
    return transport_ptr;
}

void GraphicTransport::add_pawn(GraphicPawn *pawn)
{
    pawn_list.append(pawn);
}

QList<GraphicPawn*> GraphicTransport::get_pawn_list()
{
    return pawn_list;
}

bool GraphicTransport::is_full()
{
    if (transport_ptr->getTransportType().compare("boat")==0 && pawn_list.size()<3)
    {
        return true;
    }
    if (transport_ptr->getTransportType().compare("dolpin")==0 && pawn_list.size()==0)
    {
        return true;
    }

    return false;
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
