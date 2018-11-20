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

    std::shared_ptr<Common::Pawn> get_pawn();
    std::string getName();

public slots:
    void allow_movement(bool allowed, std::list<int> pawn_id);

signals:
    void pawn_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    std::shared_ptr<Common::Pawn> pawn_ptr;
    QVector<QPointF> vertex;
    QGraphicsItem* parent;
    QColor color;
    QBrush brush;
    std::string owner;
    QPointF old_pos;
    bool movement_allowed;
};


#endif // GRAPHICPAWN_H
