#ifndef GRAPHICTRANSPORT_H
#define GRAPHICTRANSPORT_H

#include <QGraphicsPolygonItem>
#include<QVector>
#include<QPointF>
#include<transport.hh>
#include "graphicpawn.h"

class GraphicTransport: public QObject, public QGraphicsPolygonItem{
    Q_OBJECT
public:

    GraphicTransport(std::shared_ptr<Common::Transport> transport_ptr, double scale, QGraphicsItem* parent=nullptr);

    QGraphicsItem* parent;

    std::shared_ptr<Common::Transport> get_transport();
    void add_pawn(GraphicPawn* pawn);
    void remove_pawn(GraphicPawn* pawn);
    QList<GraphicPawn*> get_pawn_list();

    bool is_full();

public slots:
    void allow_movement(bool allowed);

signals:
    void transport_is_moved(int transport_id, QPointF old_pos, QPointF new_pos);

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
    QList<GraphicPawn*> pawn_list;
    bool movement_allowed;

};

#endif // GRAPHICTRANSPORT_H
