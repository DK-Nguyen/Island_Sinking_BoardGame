#include <graphicpawn.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <QBrush>
#include <QLineF>
#include <QDrag>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <pawn.hh>
#include <iostream>

UI::GraphicPawn::GraphicPawn(std::shared_ptr<Common::Pawn> pawnPtr,
                         double scale,
                         QColor color, std::string owner,
                         QGraphicsItem* parent)
    : QGraphicsPolygonItem (parent)
{

    QVector<QPointF> pawnVertex;
    pawnVertex << QPointF(0,0)*scale;
    pawnVertex << QPointF(1,0)*scale;
    pawnVertex << QPointF(1,1)*scale;
    pawnVertex << QPointF(0,1)*scale;

    this->pawnPtr_ = pawnPtr;
    this->parent = parent;
    this->color = color;
    this->owner = owner;
    this->movementAllowed_ = false;

    // create a polygon with vertex
    QPolygonF polygon(pawnVertex);

    // draw the poly
    setPolygon(polygon);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(this->color);
    setBrush(brush);

    setToolTip(QString::fromUtf8(this->owner.c_str()) + tr(":") + QString::number(pawnPtr->getId()));

}


std::shared_ptr<Common::Pawn> UI::GraphicPawn::getPawn()
{
    return pawnPtr_;
}

void UI::GraphicPawn::allowMovement(bool allowed, std::list<int> pawnId)
{
    for (auto id : pawnId)
    {
        if (id == pawnPtr_->getId())
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

            break;
        }
    }
}

void UI::GraphicPawn::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ClosedHandCursor);
    old_pos = this->scenePos();
    QGraphicsItem::mousePressEvent(event);
}

void UI::GraphicPawn::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{   
    QGraphicsItem::mouseMoveEvent(event);
}

void UI::GraphicPawn::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    setCursor(Qt::OpenHandCursor);
    QPointF new_pos = this->scenePos();
    emit pawnIsMoved(this->pawnPtr_->getId(), old_pos, new_pos);
    std::cerr << "old pos: " << old_pos.rx() << ", " << old_pos.ry() << "\n";
    std::cerr << "new pos: " << new_pos.rx() << ", " << new_pos.ry() << "\n";
}
