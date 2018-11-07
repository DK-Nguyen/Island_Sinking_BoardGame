#ifndef GRAPHICHEX_H
#define GRAPHICHEX_H

#include <QGraphicsPolygonItem>
#include<hex.hh>
#include<QVector>
#include<QPointF>

class GraphicHex: public QGraphicsPolygonItem{
public:

    GraphicHex(std::shared_ptr<Common::Hex> hex_ptr, QVector<QPointF> vertex, QGraphicsItem* parent=nullptr);
    ~GraphicHex();
    std::shared_ptr<Common::Hex> hex_ptr;
    QVector<QPointF> vertex;
};


#endif // GRAPHICHEX_H
