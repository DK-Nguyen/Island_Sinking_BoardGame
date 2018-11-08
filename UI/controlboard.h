#ifndef CONTROLBOARD_H
#define CONTROLBOARD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <configurationwindow.h>
#include <button.h>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <wheel.h>
#include <QPushButton>

/***
 size: 300x720
 TODO:
 - write Wheel class
 - make board arrangement
 ***/
class ControlBoard: public QGraphicsView
{
    Q_OBJECT
public:
    // constructor
    ControlBoard(QSharedPointer<QHash<QString, int>> current_points,
                 QSharedPointer<QHash<QString, int>> top10,
                 QWidget* parent);

    ~ControlBoard();

    void update_current_turn(QString current_player);
    void update_stage(QString current_stage);
    void update_movement_left(int movement_left);
    void update_point(QStringList player_names); // this also handles updating top10 list if one of players exceed the 10-th result

signals:
    void start_game();

public slots:
    void on_closing();

private slots:
    void save_game();
    void spin_wheel();
    void deactivate_start_button();


private:
    // data structure that keeps items
    QGraphicsScene* scene;

    // players' point
    QSharedPointer<QHash<QString, int>> points;

    // top10 list
    QSharedPointer<QHash<QString, int>> top10;

    // configuration
    QSharedPointer<Configuration> config;

    // buttons
    QPushButton* start_button;
    QPushButton* save_button;
    QPushButton* quit_button;

    // current turn, stage and movement left text
    QGraphicsTextItem *current_turn, *stage, *movement_left;

    // player points, top10 text
    QGraphicsTextItem *points_title, *top10_title;
    QHash<QString, QGraphicsTextItem*> points_txt;

    // wheel
    Wheel* wheel;

    // wheel_output and bounding box
    QGraphicsTextItem* wheel_output;
    QGraphicsRectItem* wheel_bb;

    void initialize_points();
    void show_top10();

};

#endif // CONTROLBOARD_H
