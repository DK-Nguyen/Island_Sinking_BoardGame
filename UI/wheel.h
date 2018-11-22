#ifndef WHEEL_H
#define WHEEL_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class Wheel: public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    // constructor
    Wheel(QString filename, int no_rotation, int interval, bool has_clock_direction, QObject* parent=nullptr);

    // destructor
    ~Wheel();

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void set_click_action(bool flag);


signals:
    void clicked();

public slots:
    void spin(int target_output);


private:
    QPixmap image;
    int no_rotation, interval;
    bool has_clock_direction;


    bool can_click;

    // spin animation
    void spin_animation(int degree, int target_degree);

};

#endif // WHEEL_H
