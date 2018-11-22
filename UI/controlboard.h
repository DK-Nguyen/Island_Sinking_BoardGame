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
#include <gamestate.hh>

/***
 size: 300x720
 TODO:
 - implement save current game
 - update_top10() relies on condition that 10 textItem were created -> ensure top10 variable has 10 element in mainwindow

 ***/

const int WIDTH = 350;
const int HEIGHT = 720;
const std::pair<int, int> BUTTON_SIZE = std::make_pair(70,35);
const QPointF PLAY_BUTTON_POS = QPointF(35,15);
const QPointF SAVE_BUTTON_POS = QPointF(140,15);
const QPointF QUIT_BUTTON_POS = QPointF(245,15);
const QPointF TURN_POS = QPointF(95,70);
const QPointF STAGE_POS = QPointF(105,100);
const QPointF ACTION_POS = QPointF(115,130);
const QPointF POINT_TT_POS = QPointF(15,170);
const QPointF TOP10_TT_POS = QPointF(190,170);
const QPointF POINT_POS = QPointF(15,200);
const QPointF TOP10_POS = QPointF(190,200);
const QPointF WHEEL_POS = QPointF(40,500);



class ControlBoard: public QGraphicsView
{
    Q_OBJECT
public:
    // constructor
    ControlBoard(std::shared_ptr<Common::IGameRunner> game_engine,
                 std::shared_ptr<GameState> game_state,
                 double board_scale,
                 QWidget* parent);


    ~ControlBoard();

signals:
    void animate_inner_wheel(int target_degree); // signal caught by Wheel object
    void animate_outter_wheel(int target_degree);  // signal caught by Wheel object
    void control_board_close(); // signal caught by parent object
    void wheel_spin(std::pair<std::string, std::string> wheel_output);


public slots:
    void update_current_turn(); // allow parent to update current turn
    void update_stage(); // allow parent to update current stage
    void update_movement_left(); // allow parent to update movement left
    void update_point(std::vector<int> player_IDs); // allow parent to update points
    void update_top10();
    void set_wheel_click(bool flag);

private slots:
    void save_button_clicked(); // perform saving game & emit control_board_close() to parent & call close()
    void deactivate_play_button(); // disable play button after being clicked
    void wheel_clicked(); // perform wheel spinning & emit animation signals to Wheel objects
    void quit_game(); // emit control_board_close() to parent and call close()
    void quit_button_clicked(); // proxy to call closeEvent()


private:
    std::shared_ptr<Common::IGameRunner> game_engine;
    std::shared_ptr<GameState> game_state;
    double board_scale;

    // data structure that keeps items
    QGraphicsScene* scene;

    bool allow_quit = false;

    // buttons
    QPushButton* play_button;
    QPushButton* save_button;
    QPushButton* quit_button;

    // current turn, stage and movement left text
    QGraphicsTextItem *current_turn, *stage, *movement_left;

    // player points, top10 text
    QGraphicsTextItem *points_title, *top10_title;
    QHash<QString, QGraphicsTextItem*> points_txt;
    QVector<QGraphicsTextItem*> top10_txt;

    // wheel
    Wheel *inner_wheel, *outter_wheel;
    QGraphicsPolygonItem* marker;


    void initialize_points();
    void initialize_top10();
    void initialize_inner_wheel();
    void initialize_outter_wheel();
};

#endif // CONTROLBOARD_H
