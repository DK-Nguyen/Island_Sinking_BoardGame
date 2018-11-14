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

GraphicPawn::GraphicPawn(std::shared_ptr<Common::Pawn> pawn_ptr,
                         QVector<QPointF> vertex,
                         QColor color, std::string owner,
                         QGraphicsItem* parent)
    : QGraphicsPolygonItem (parent)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemIsMovable);

    this->vertex = vertex;
    this->pawn_ptr = pawn_ptr;
    this->parent = parent;
    this->color = color;
    this->owner = owner;

    // create a polygon with vertex
    QPolygonF polygon(vertex);

    // draw the poly
    setPolygon(polygon);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(this->color);
    setBrush(brush);

    setToolTip(QString::fromUtf8(this->owner.c_str()));

}


std::shared_ptr<Common::Pawn> GraphicPawn::get_pawn()
{
    return pawn_ptr;
}

QGraphicsItem *GraphicPawn::get_parent()
{
    return parent;
}

void GraphicPawn::set_parent(QGraphicsItem *parent)
{
    this->parent = parent;
}

void GraphicPawn::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ClosedHandCursor);
    old_pos = this->scenePos();
    QGraphicsItem::mousePressEvent(event);
}

void GraphicPawn::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{   
    QGraphicsItem::mouseMoveEvent(event);
}

void GraphicPawn::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    setCursor(Qt::OpenHandCursor);
    QPointF new_pos = this->scenePos();
    emit pawn_is_moved(this->pawn_ptr->getId(), old_pos, new_pos);
    std::cerr << "old pos: " << old_pos.rx() << ", " << old_pos.ry() << "\n";
    std::cerr << "new pos: " << new_pos.rx() << ", " << new_pos.ry() << "\n";
}
