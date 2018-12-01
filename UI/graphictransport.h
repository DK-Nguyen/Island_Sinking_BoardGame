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

    // initialize with transport ptr and board scale
    GraphicTransport(std::shared_ptr<Common::Transport> transportPtr, double scale, QGraphicsItem* parent=nullptr);

    QGraphicsItem* parent;

    // get Common::Transport
    std::shared_ptr<Common::Transport> getTransport();

    // add graphic pawn to transport
    void addPawn(UI::GraphicPawn* pawn);

    // remove graphic pawn from transport
    void removePawn(UI::GraphicPawn* pawn);

    // get list of graphic pawn
    QList<UI::GraphicPawn*> getPawnList();

    // check if transport is full
    bool isFull();

public slots:

    // slot to set/unset movement
    void allowMovement(bool allowed);

signals:

    // signal sent to hexBoard when transport is moved
    void transportIsMoved(int transport_id, QPointF old_pos, QPointF new_pos);

protected:

    // handle mouse events
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
