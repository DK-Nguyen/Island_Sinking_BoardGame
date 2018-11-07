#include <graphichex.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>

GraphicHex::GraphicHex(std::shared_ptr<Common::Hex> hex_ptr, QVector<QPointF> vertex, QGraphicsItem* parent)
{
    this->vertex = vertex;
    this->hex_ptr = hex_ptr;

    // create a polygon with vertex
    QPolygonF hexagon(vertex);

    // draw the poly
    setPolygon(hexagon);
}

GraphicHex::~GraphicHex()
{
    return;
}


