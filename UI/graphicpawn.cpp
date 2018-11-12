#include <graphicpawn.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <QBrush>

GraphicPawn::GraphicPawn(std::shared_ptr<Common::Pawn> pawn_ptr,
                         QVector<QPointF> vertex,
                         QColor color, std::string owner,
                         QGraphicsItem* parent)
    : QGraphicsPolygonItem (parent)
{
    this->vertex = vertex;
    this->pawn_ptr = pawn_ptr;
    this->parent = parent;
    this->color = color;
    this->owner = owner;

    // create a polygon with vertex
    QPolygonF triangle(vertex);

    // draw the poly
    setPolygon(triangle);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(this->color);
    setBrush(brush);

    setToolTip(QString::fromUtf8(this->owner.c_str()));

}

GraphicPawn::~GraphicPawn()
{
    return;
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
