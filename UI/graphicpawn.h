#ifndef GRAPHICPAWN_H
#define GRAPHICPAWN_H

#include <QGraphicsPolygonItem>
#include<hex.hh>
#include<QVector>
#include<QPointF>

class GraphicPawn: public QGraphicsPolygonItem{
public:

    GraphicPawn(std::shared_ptr<Common::Pawn> pawn_ptr, QVector<QPointF> vertex, QGraphicsItem* parent=nullptr);
    ~GraphicPawn();

    std::shared_ptr<Common::Pawn> pawn_ptr;
    QVector<QPointF> vertex;
    QGraphicsItem* parent;
};


#endif // GRAPHICPAWN_H
