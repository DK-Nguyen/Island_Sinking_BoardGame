#ifndef GRAPHICACTOR_H
#define GRAPHICACTOR_H

#include <QGraphicsPixmapItem>
#include<QVector>
#include<QPointF>
#include<actor.hh>

namespace UI{
class GraphicActor: public QObject, public QGraphicsPixmapItem{
    Q_OBJECT
public:

    GraphicActor(std::shared_ptr<Common::Actor> actorPtr, QGraphicsItem* parent=nullptr);

    QGraphicsItem* parent;

    std::shared_ptr<Common::Actor> getActor();

signals:
    // notify HexBoard of actor movement-> HexBoard need to reset to old position if move is not valid
    void actorIsMoved(int actor_id, QPointF old_pos, QPointF new_pos);

public slots:
    // enable movement of actors in appropriate stage
    void allowMovement(bool allowed, std::list<int> actor_id);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QPixmap image;
    QPointF old_pos;
    std::string actorType_;
    bool movementAllowed_;
    std::shared_ptr<Common::Actor> actorPtr_;
};
}

#endif // GRAPHICACTOR_H
