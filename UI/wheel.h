#ifndef WHEEL_H
#define WHEEL_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

namespace  UI{
class Wheel: public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    // constructor
    Wheel(QString filename, int noRotation, int interval_, bool hasClockDirection, QObject* parent=nullptr);

    // destructor
    ~Wheel();

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void setClickAction(bool flag);


signals:
    void clicked();

public slots:
    void spin(int targetDegree);


private:
    QPixmap image;
    int noRotation_, interval_;
    bool hasClockDirection_;


    bool canClick_;

    // spin animation
    void spinAnimation(int degree, int targetDegree);

};
}

#endif // WHEEL_H
