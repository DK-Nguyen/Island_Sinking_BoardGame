#include <wheel.h>
#include <QTimer>
#include <iostream>

UI::Wheel::Wheel(QString filename, int noRotation, int interval, bool hasClockDirection, QObject *parent)
    :QObject (parent)
{
    this->noRotation_ = noRotation;
    this->interval_ = interval;
    this->hasClockDirection_ = hasClockDirection;
    this->canClick_ = false;

    image = QPixmap(filename);
    setPixmap(image);
    setTransformOriginPoint(image.rect().center());

}

UI::Wheel::~Wheel()
{
    return;
}

void UI::Wheel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (canClick_)
    {
        emit clicked();
    }
}

void UI::Wheel::spin(int targetDegree)
{
    if (hasClockDirection_)
    {
        if (targetDegree < 0)
        {
            targetDegree = 360 - targetDegree;
        }
    }
    else
    {
        if (targetDegree > 0)
        {
            targetDegree = targetDegree - 360;
        }
    }


    setRotation(targetDegree);
    spinAnimation(targetDegree, targetDegree + 360*noRotation_);

    return ;
}

void UI::Wheel::setClickAction(bool flag)
{
    canClick_ = flag;
}


void UI::Wheel::spinAnimation(int degree, int targetDegree)
{
    setRotation(degree);
    if (hasClockDirection_)
    {
        if (degree < targetDegree)
        {
            int target = std::min(targetDegree, degree+30);
            QTimer::singleShot(interval_, [=]{spinAnimation(target, targetDegree);});
        }
    }
    else
    {
        if (degree > -targetDegree)
        {
            int target = std::max(-targetDegree, degree-30);
            QTimer::singleShot(interval_, [=]{spinAnimation(target, targetDegree);});
        }
    }
}
