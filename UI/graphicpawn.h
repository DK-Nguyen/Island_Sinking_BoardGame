#ifndef GRAPHICPAWN_H
#define GRAPHICPAWN_H

#include <QGraphicsPolygonItem>
#include<hex.hh>
#include<QVector>
#include<QPointF>
#include<QBrush>


namespace UI{
class GraphicPawn: public QObject, public QGraphicsPolygonItem{
    Q_OBJECT
public:

    // initialize with pawn ptr, board scale, color, player's name
    GraphicPawn(std::shared_ptr<Common::Pawn> pawnPtr,
                double scale,
                QColor color,
                std::string owner,
                QGraphicsItem* parent=nullptr);

    // get Common::Pawn
    std::shared_ptr<Common::Pawn> getPawn();

public slots:
    // slot to set movement of graphic pawn
    void allowMovement(bool allowed, std::list<int> pawn_id);

signals:

    // signal sent to hexBoard when graphic pawn is moved
    void pawnIsMoved(int pawn_id, QPointF old_pos, QPointF new_pos);

protected:
    // handling mouse events
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    std::shared_ptr<Common::Pawn> pawnPtr_;
    QGraphicsItem* parent;
    QColor color;
    QBrush brush;
    std::string owner;
    QPointF old_pos;
    bool movementAllowed_;
};
}

#endif // GRAPHICPAWN_H
