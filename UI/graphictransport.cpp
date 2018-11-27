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

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    auto transport_type = transport_ptr->getTransportType();
    QVector<QPointF> vertices;

    setToolTip(QString::number(transport_ptr->getId()));

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
    brush.setColor(Qt::white);
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

void GraphicTransport::remove_pawn(GraphicPawn *pawn)
{
    auto it = pawn_list.begin();
    while (it!= pawn_list.end() && (*it)->get_pawn()->getId() != pawn->get_pawn()->getId())
    {
        it++;
    }
    pawn_list.erase(it);
}

QList<GraphicPawn*> GraphicTransport::get_pawn_list()
{
    return pawn_list;
}

bool GraphicTransport::is_full()
{
    std::cerr << "number of pawns on graphic transport: " << pawn_list.size() << "\n";
    std::cerr << "number of pawns on backend transport: " << transport_ptr->getPawnsInTransport().size() << "\n";

    if (transport_ptr->getTransportType().compare("boat")==0 && pawn_list.size()<3)
    {
        return false;
    }
    if (transport_ptr->getTransportType().compare("dolphin")==0 && pawn_list.size()==0)
    {
        return false;
    }

    return true;
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
    emit transport_is_moved(transport_ptr->getId(), old_pos, new_pos);
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
