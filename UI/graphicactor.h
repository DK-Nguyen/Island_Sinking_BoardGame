#ifndef GRAPHICACTOR_H
#define GRAPHICACTOR_H

#include <QGraphicsPixmapItem>
#include<QVector>
#include<QPointF>
#include<actor.hh>

class GraphicActor: public QObject, public QGraphicsPixmapItem{
    Q_OBJECT
public:

    GraphicActor(std::shared_ptr<Common::Actor> actor_ptr, QGraphicsItem* parent=nullptr);
    std::shared_ptr<Common::Actor> actor_ptr;
    QGraphicsItem* parent;

signals:
    void actor_is_moved(int actor_id, QPointF old_pos, QPointF new_pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;


private:
    QPixmap image;
    QPointF old_pos;

};


#endif // GRAPHICACTOR_H
