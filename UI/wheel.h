#ifndef WHEEL_H
#define WHEEL_H

#include <QGraphicsPolygonItem>
#include <QGraphicsSceneMouseEvent>

class Wheel: public QGraphicsPixmapItem
{
    // constructor
    Wheel(int x_pos, int y_pos, int radius, QGraphicsItem* parent=nullptr);

    // spin animation
    void spin_animation();


private:
    int x_pos, y_pos, radius;

};

#endif // WHEEL_H
