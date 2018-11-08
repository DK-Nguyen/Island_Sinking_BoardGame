#include <wheel.h>

// TODO: plot the wheel
// create animation

Wheel::Wheel(int x_pos, int y_pos, int radius, QGraphicsItem* parent)
    : QGraphicsPixmapItem (parent)
{
    this->x_pos = x_pos;
    this->y_pos = y_pos;
    this->radius = radius;

    // perform plotting the wheel
}


void Wheel::spin_animation()
{
    // TODO: implement animation
    return;
}
