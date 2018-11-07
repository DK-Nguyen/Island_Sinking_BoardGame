#include <graphicpawn.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>

GraphicPawn::GraphicPawn(std::shared_ptr<Common::Pawn> pawn_ptr, QVector<QPointF> vertex, QGraphicsItem* parent)
{
    this->vertex = vertex;
    this->pawn_ptr = pawn_ptr;
    this->parent = parent;

    // create a polygon with vertex
    QPolygonF triangle(vertex);

    // draw the poly
    setPolygon(triangle);
}

GraphicPawn::~GraphicPawn()
{
    return;
}
