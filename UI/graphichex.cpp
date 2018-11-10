#include <graphichex.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <QBrush>
#include <QColor>

GraphicHex::GraphicHex(std::shared_ptr<Common::Hex> hex_ptr, QVector<QPointF> vertex, QGraphicsItem* parent): QGraphicsPolygonItem (parent)
{
    this->vertex = vertex;
    this->hex_ptr = hex_ptr;

    // create a polygon with vertex
    QPolygonF hexagon(vertex);

    // draw the poly
    setPolygon(hexagon);
    set_color(hex_ptr->getPieceType());
}

// perform flipping of hex tile
void GraphicHex::flip()
{
    return;
}

void GraphicHex::set_color(std::string type)
{
    QColor color;

    if (type.compare("Peak")==0)
    {
        color = Qt::gray;
    }
    else if (type.compare("Mountain")==0)
    {
        color = Qt::lightGray;
    }
    else if (type.compare("Forest")==0)
    {
        color = Qt::green;
    }
    else if (type.compare("Beach")==0)
    {
        color = Qt::yellow;
    }
    else if (type.compare("Water")==0)
    {
        color = Qt::blue;
    }
    else
    {
        color = Qt::magenta;
    }

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    setBrush(brush);

}
GraphicHex::~GraphicHex()
{
    return;
}


