#ifndef GRAPHICTRANSPORT_H
#define GRAPHICTRANSPORT_H

#include <QGraphicsPixmapItem>
#include<QVector>
#include<QPointF>
#include<transport.hh>
#include "graphicpawn.h"

class GraphicTransport: public QObject, public QGraphicsPixmapItem{
    Q_OBJECT
public:

    GraphicTransport(std::shared_ptr<Common::Transport> transport_ptr, QGraphicsItem* parent=nullptr);

    QGraphicsItem* parent;

    std::shared_ptr<Common::Transport> get_transport();
    std::string getName();

public slots:
    void allow_movement(bool allowed);

signals:
    void transport_is_moved(int actor_id, QPointF old_pos, QPointF new_pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    std::shared_ptr<Common::Transport> transport_ptr;
    QPixmap image;
    QPointF old_pos;
    QVector<GraphicPawn*> pawn_list;
    bool movement_allowed;

};

#endif // GRAPHICTRANSPORT_H
