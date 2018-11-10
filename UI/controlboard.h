#ifndef CONTROLBOARD_H
#define CONTROLBOARD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <configurationwindow.h>
#include <button.h>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QPushButton>
#include <wheel.h>
#include <igamerunner.hh>

/***
 size: 300x720
 TODO:
 - correct statistic & game state initialize in constructor
 - write statistic update slots

 ***/
class ControlBoard: public QGraphicsView
{
    Q_OBJECT
public:
    // constructor
    ControlBoard(std::shared_ptr<Common::IGameRunner> game_engine,
                 QSharedPointer<QHash<QString, int>> current_points,
                 QSharedPointer<QHash<QString, int>> top10,
                 QWidget* parent);

    ~ControlBoard();

signals:
    void animate_inner_wheel(int target_degree); // signal caught by Wheel object
    void animate_outter_wheel(int target_degree);  // signal caught by Wheel object
    void control_board_close(); // signal caught by parent object

public slots:
    void update_current_turn(QString current_player); // allow parent to update current turn
    void update_stage(QString current_stage); // allow parent to update current stage
    void update_movement_left(int movement_left); // allow parent to update movement left
    void update_point(QStringList player_names); // allow parent to update points and top10

private slots:
    void save_button_clicked(); // perform saving game & emit control_board_close() to parent & call close()
    void deactivate_play_button(); // disable play button after being clicked
    void wheel_clicked(); // perform wheel spinning & emit animation signals to Wheel objects
    void quit_game(); // emit control_board_close() to parent and call close()
    void quit_button_clicked(); // proxy to call closeEvent()


private:
    std::shared_ptr<Common::IGameRunner> game_engine;
    // data structure that keeps items
    QGraphicsScene* scene;

    bool allow_quit = false;

    // players' point
    QSharedPointer<QHash<QString, int>> points;

    // top10 list
    QSharedPointer<QHash<QString, int>> top10;

    // configuration
    QSharedPointer<Configuration> config;

    // buttons
    QPushButton* play_button;
    QPushButton* save_button;
    QPushButton* quit_button;

    // current turn, stage and movement left text
    QGraphicsTextItem *current_turn, *stage, *movement_left;

    // player points, top10 text
    QGraphicsTextItem *points_title, *top10_title;
    QHash<QString, QGraphicsTextItem*> points_txt;

    // wheel
    Wheel *inner_wheel, *outter_wheel;
    QGraphicsPolygonItem* marker;


    void initialize_points();
    void show_top10();
    void initialize_inner_wheel();
    void initialize_outter_wheel();

    void closeEvent(QCloseEvent* event);
};

#endif // CONTROLBOARD_H
