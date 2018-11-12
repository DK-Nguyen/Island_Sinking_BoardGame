#ifndef GRAPHICPAWN_H
#define GRAPHICPAWN_H

#include <QGraphicsPolygonItem>
#include<hex.hh>
#include<QVector>
#include<QPointF>
#include<QBrush>

class GraphicPawn: public QObject, public QGraphicsPolygonItem{
    Q_OBJECT
public:

    GraphicPawn(std::shared_ptr<Common::Pawn> pawn_ptr,
                QVector<QPointF> vertex,
                QColor color,
                std::string owner,
                QGraphicsItem* parent=nullptr);

    ~GraphicPawn();

    std::shared_ptr<Common::Pawn> get_pawn();
    QGraphicsItem* get_parent();
    void set_parent(QGraphicsItem* parent);

private:
    std::shared_ptr<Common::Pawn> pawn_ptr;
    QVector<QPointF> vertex;
    QGraphicsItem* parent;
    QColor color;
    QBrush brush;
    std::string owner;
};


#endif // GRAPHICPAWN_H
