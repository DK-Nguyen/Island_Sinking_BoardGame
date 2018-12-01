#include <graphichex.h>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <QBrush>
#include <QColor>
#include <QGraphicsSceneMouseEvent>

UI::GraphicHex::~GraphicHex()
{
    return;
}

UI::GraphicHex::GraphicHex(std::shared_ptr<Common::Hex> hexPtr, QVector<QPointF> vertex, int id, QGraphicsItem* parent)
    : QGraphicsPolygonItem (parent)
{
    this->vertex = vertex;
    this->hexPtr_ = hexPtr;
    this->id_ = id;

    // create a polygon with vertex
    QPolygonF hexagon(vertex);

    // draw the poly
    setPolygon(hexagon);
    setColor(hexPtr->getPieceType());
    setAcceptDrops(true);
}

// perform flipping of hex tile
void UI::GraphicHex::flip()
{
    setColor("Water");
    return;
}

void UI::GraphicHex::setColor(std::string type)
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
        color = QColor(0, 191, 255);
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

std::shared_ptr<Common::Hex> UI::GraphicHex::getHex()
{
    return hexPtr_;
}

QVector<QPointF> UI::GraphicHex::getVertex()
{
    return vertex;
}

void UI::GraphicHex::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked(id_);
}

void UI::GraphicHex::dropEvent(QGraphicsSceneDragDropEvent *event)
{
     event->setAccepted(true);
     update();
}


void UI::GraphicHex::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(true);
    update();
}

void UI::GraphicHex::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    update();
}




