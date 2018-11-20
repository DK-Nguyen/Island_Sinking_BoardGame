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

    QGraphicsItem* parent;

    std::shared_ptr<Common::Actor> get_actor();
    std::string getName();

signals:
    // notify HexBoard of actor movement-> HexBoard need to reset to old position if move is not valid
    void actor_is_moved(int actor_id, QPointF old_pos, QPointF new_pos);

public slots:
    // enable movement of actors in appropriate stage
    void allow_movement(bool allowed, std::list<int> actor_id);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPixmap image;
    QPointF old_pos;
    std::string actor_type;
    bool movement_allowed;
    std::shared_ptr<Common::Actor> actor_ptr;
};


#endif // GRAPHICACTOR_H
