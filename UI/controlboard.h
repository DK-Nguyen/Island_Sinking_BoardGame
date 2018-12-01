#ifndef CONTROLBOARD_H
#define CONTROLBOARD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <configurationwindow.h>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QPushButton>
#include <wheel.h>
#include <igamerunner.hh>
#include <gamestate.hh>


const int WIDTH = 350;
const int HEIGHT = 720;
const std::pair<int, int> BUTTON_SIZE = std::make_pair(70,35);
const QPointF QUIT_BUTTON_POS = QPointF(140,15);
const QPointF TURN_POS = QPointF(95,70);
const QPointF STAGE_POS = QPointF(105,100);
const QPointF ACTION_POS = QPointF(115,130);
const QPointF POINT_TT_POS = QPointF(15,170);
const QPointF TOP10_TT_POS = QPointF(190,170);
const QPointF POINT_POS = QPointF(15,200);
const QPointF TOP10_POS = QPointF(190,200);
const QPointF WHEEL_POS = QPointF(40,500);


namespace UI{

class ControlBoard: public QGraphicsView
{
    Q_OBJECT

public:

    ControlBoard(std::shared_ptr<Common::IGameRunner> gameEngine_,
                 std::shared_ptr<Student::GameState> gameState_,
                 double boardScale_,
                 QWidget* parent);


    void clear();

signals:
    // signal emit to object
    void animateInnerWheel(int target_degree);

    // signal emit to Wheel object
    void animateOutterWheel(int target_degree);

    // signal emit to mainWindow
    void controlBoardClose();

    //signal emit to Wheel object
    void wheelSpin(std::pair<std::string, std::string> wheel_output);


public slots:
    // slot to update current turn
    void updateCurrentTurn();

    // slot to update curent game stage
    void updateStage();

    // slot to update number of move left
    void updateMovementLeft();

    // slot to update points
    void updatePoint(std::vector<int> player_IDs);

    // slot to update top10
    void updateTop10();

    // slot to enable wheel click
    void setWheelClick(bool flag);

private slots:

    // slot to perform wheel action + animation
    void wheelClicked();

    // slot handle quit button clicked
    void quitButtonClicked();

private:
    std::shared_ptr<Common::IGameRunner> gameEngine_;
    std::shared_ptr<Student::GameState> gameState_;
    double boardScale_;

    // data structure that keeps graphic items
    QGraphicsScene* scene_;

    bool allowQuitting_ = false;

    // buttons
    QPushButton* playButton_;
    QPushButton* quitButton_;

    // current turn, stage and movement left text
    QGraphicsTextItem *currentTurn_, *stage_, *movementLeft_;

    // player points, top10 text
    QGraphicsTextItem *pointTitle_, *top10Title_;
    QHash<QString, QGraphicsTextItem*> pointTxt_;
    QVector<QGraphicsTextItem*> top10Txt_;

    // wheel
    UI::Wheel *innerWheel_, *outterWheel_;
    QGraphicsPolygonItem* marker_;

    // show points of players
    void initializePoints();

    // show top10 list
    void initializeTop10();

    // show inner wheel
    void initializeInnerWheel();

    // show outter wheel
    void initializeOutterWheel();
};
}

#endif // CONTROLBOARD_H
