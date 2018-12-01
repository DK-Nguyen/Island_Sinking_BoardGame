#include <graphictransport.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>


UI::GraphicTransport::GraphicTransport(std::shared_ptr<Common::Transport> transportPtr, double scale, QGraphicsItem* parent)
    : QGraphicsPolygonItem(parent)
{

    this->transportPtr_ = transportPtr;
    this->parent = parent;
    this->movementAllowed_ = false;

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    auto transportType = transportPtr->getTransportType();
    QVector<QPointF> vertices;

    setToolTip(QString::number(transportPtr->getId()));

    if (transportType.compare("boat")==0)
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

std::shared_ptr<Common::Transport> UI::GraphicTransport::getTransport()
{
    return transportPtr_;
}

void UI::GraphicTransport::addPawn(UI::GraphicPawn *pawn)
{
    pawnList_.append(pawn);
}

void UI::GraphicTransport::removePawn(UI::GraphicPawn *pawn)
{
    auto it = pawnList_.begin();
    while (it!= pawnList_.end() && (*it)->getPawn()->getId() != pawn->getPawn()->getId())
    {
        it++;
    }
    pawnList_.erase(it);
}

QList<UI::GraphicPawn*> UI::GraphicTransport::getPawnList()
{
    return pawnList_;
}

bool UI::GraphicTransport::isFull()
{
    std::cerr << "number of pawns on graphic transport: " << pawnList_.size() << "\n";
    std::cerr << "number of pawns on backend transport: " << transportPtr_->getPawnsInTransport().size() << "\n";

    if (transportPtr_->getTransportType().compare("boat")==0 && pawnList_.size()<3)
    {
        return false;
    }
    if (transportPtr_->getTransportType().compare("dolphin")==0 && pawnList_.size()==0)
    {
        return false;
    }

    return true;
}


void UI::GraphicTransport::allowMovement(bool allowed)
{
    movementAllowed_ = allowed;

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


void UI::GraphicTransport::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    old_pos = this->scenePos();
    QGraphicsItem::mousePressEvent(event);
}

void UI::GraphicTransport::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void UI::GraphicTransport::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    QPointF new_pos = this->scenePos();
    emit transportIsMoved(transportPtr_->getId(), old_pos, new_pos);
}

void UI::GraphicTransport::dropEvent(QGraphicsSceneDragDropEvent *event)
{
     event->setAccepted(true);
     update();
}

void UI::GraphicTransport::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(true);
    update();
}

void UI::GraphicTransport::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    update();
}
