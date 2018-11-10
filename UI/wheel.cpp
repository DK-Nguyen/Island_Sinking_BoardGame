#include <wheel.h>
#include <QTimer>
#include <iostream>

Wheel::Wheel(QString filename, int no_rotation, int interval, bool has_clock_direction, QObject *parent)
    :QObject (parent)
{
    this->no_rotation = no_rotation;
    this->interval = interval;
    this->has_clock_direction = has_clock_direction;

    image = QPixmap(filename);
    setPixmap(image);
    setTransformOriginPoint(image.rect().center());

}

Wheel::~Wheel()
{
    return;
}

void Wheel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked();
}

void Wheel::spin(int target_degree)
{
    if (has_clock_direction)
    {
        if (target_degree < 0)
        {
            target_degree = 360 - target_degree;
        }
    }
    else
    {
        if (target_degree > 0)
        {
            target_degree = target_degree - 360;
        }
    }


    setRotation(target_degree);
    spin_animation(target_degree, target_degree + 360*5);

    return ;
}


void Wheel::spin_animation(int degree, int target_degree)
{
    setRotation(degree);
    if (has_clock_direction)
    {
        if (degree < target_degree)
        {
            int target = std::min(target_degree, degree+30);
            QTimer::singleShot(interval, [=]{spin_animation(target, target_degree);});
        }
    }
    else
    {
        if (degree > -target_degree)
        {
            int target = std::max(-target_degree, degree-30);
            QTimer::singleShot(interval, [=]{spin_animation(target, target_degree);});
        }
    }
}
