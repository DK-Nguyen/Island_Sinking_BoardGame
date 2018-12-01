#ifndef GRAPHICTRANSPORT_H
#define GRAPHICTRANSPORT_H

#include <QGraphicsPolygonItem>
#include<QVector>
#include<QPointF>
#include<transport.hh>
#include "graphicpawn.h"

namespace UI{
class GraphicTransport: public QObject, public QGraphicsPolygonItem{
    Q_OBJECT
public:

    GraphicTransport(std::shared_ptr<Common::Transport> transportPtr, double scale, QGraphicsItem* parent=nullptr);

    QGraphicsItem* parent;

    std::shared_ptr<Common::Transport> getTransport();
    void addPawn(UI::GraphicPawn* pawn);
    void removePawn(UI::GraphicPawn* pawn);
    QList<UI::GraphicPawn*> getPawnList();

    bool isFull();

public slots:
    void allowMovement(bool allowed);

signals:
    void transportIsMoved(int transport_id, QPointF old_pos, QPointF new_pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    std::shared_ptr<Common::Transport> transportPtr_;
    QPixmap image;
    QPointF old_pos;
    QList<UI::GraphicPawn*> pawnList_;
    bool movementAllowed_;

};
}

#endif // GRAPHICTRANSPORT_H
