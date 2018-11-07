#ifndef GRAPHICACTOR_H
#define GRAPHICACTOR_H

#include <QGraphicsPixmapItem>
#include<QVector>
#include<QPointF>
#include<actor.hh>

class GraphicActor: public QGraphicsPixmapItem{
public:

    GraphicActor(std::shared_ptr<Common::Actor> actor_ptr, QGraphicsItem* parent=nullptr);
    ~GraphicActor();
    std::shared_ptr<Common::Actor> actor_ptr;
    QGraphicsItem* parent;
};


#endif // GRAPHICACTOR_H
