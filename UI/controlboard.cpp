#include <controlboard.h>
#include <QGraphicsProxyWidget>
#include <iostream>
#include <QTimer>
#include <QObject>
#include <QtWidgets>
#include <igamerunner.hh>
#include <iostream>
#include <wheel.h>

namespace UI{


ControlBoard::ControlBoard(std::shared_ptr<Common::IGameRunner> game_engine,
                           std::shared_ptr<Student::GameState> gameState_,
                           double boardScale_,
                           QWidget* parent)

    : QGraphicsView (parent)

{
    this->gameEngine_ = game_engine;
    this->gameState_ = gameState_;
    this->allowQuitting_ = false;
    this->boardScale_ = boardScale_;


    setFixedSize(int(WIDTH*boardScale_), int(HEIGHT*boardScale_));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->scene_ = new QGraphicsScene(this);
    scene_->setSceneRect(0,0,int(WIDTH*boardScale_), int(HEIGHT*boardScale_));
    setScene(scene_);
    scene_->clear();

    quitButton_ = new QPushButton(tr("Quit"));
    quitButton_->setFixedSize(int(BUTTON_SIZE.first*boardScale_), int(BUTTON_SIZE.second*boardScale_));
    auto quitButton__proxy = scene_->addWidget(quitButton_);
    quitButton__proxy->setPos(QUIT_BUTTON_POS*boardScale_);
    connect(quitButton_, SIGNAL(clicked()), this, SLOT(quitButtonClicked()));

    // handle game state display
    currentTurn_ = new QGraphicsTextItem(tr("Current Turn: ") + QString::fromUtf8(gameState_->currentPlayerName().c_str()));
    currentTurn_->adjustSize();
    currentTurn_->setPos(TURN_POS*boardScale_);
    scene_->addItem(currentTurn_);
    currentTurn_->setScale(1.1*boardScale_);
    currentTurn_->setTextWidth(-1);

    stage_ = new QGraphicsTextItem(tr("stage: ") + gameState_->currentGamePhaseName());
    stage_->adjustSize();
    stage_->setPos(STAGE_POS*boardScale_);
    scene_->addItem(stage_);
    stage_->setScale(1.1*boardScale_);
    stage_->setTextWidth(-1);

    movementLeft_ = new QGraphicsTextItem(tr("Action Left: ") + QString::number(gameState_->getActionsLeft()));
    movementLeft_->adjustSize();
    movementLeft_->setPos(ACTION_POS*boardScale_);
    scene_->addItem(movementLeft_);
    movementLeft_->setScale(1.1*boardScale_);
    movementLeft_->setTextWidth(-1);

    // handle statistics

    pointTitle_ = new QGraphicsTextItem(tr("** POINTS **"));
    pointTitle_->adjustSize();
    pointTitle_->setPos(POINT_TT_POS*boardScale_);
    scene_->addItem(pointTitle_);
    pointTitle_->setScale(1.25*boardScale_);
    pointTitle_->setTextWidth(-1);
    initializePoints();

    top10Title_ = new QGraphicsTextItem(tr("** TOP10 **"));
    top10Title_->adjustSize();
    top10Title_->setPos(TOP10_TT_POS*boardScale_);
    scene_->addItem(top10Title_);
    top10Title_->setScale(1.25*boardScale_);
    top10Title_->setTextWidth(-1);
    initializeTop10();

    // handle wheel
    initializeInnerWheel();
    initializeOutterWheel();

}

void ControlBoard::clear()
{
    pointTxt_.clear();
    top10Txt_.clear();
}

void ControlBoard::wheelClicked()
{
    auto wheelOutput = gameEngine_->spinWheel();
    int innerDegree, outterDegree;

    if (wheelOutput.first.compare("dolphin")==0)
    {
        innerDegree = 0;
    }
    else if (wheelOutput.first.compare("kraken")==0) {
        innerDegree = -90;
    }
    else if (wheelOutput.first.compare("seamunster")==0) {
        innerDegree = -180;
    }
    else{
        innerDegree = -270;
    }

    if (wheelOutput.second.compare("1")==0)
    {
        outterDegree = -90;
    }
    else if (wheelOutput.second.compare("2")==0) {
        outterDegree = 0;
    }
    else if (wheelOutput.second.compare("3")==0) {
        outterDegree = 270;
    }
    else{
        outterDegree = 180;
    }

    emit animateInnerWheel(innerDegree);
    emit animateOutterWheel(outterDegree);
    emit wheelSpin(wheelOutput);

    innerWheel_->setClickAction(false);
    outterWheel_->setClickAction(false);
}

void ControlBoard::quitButtonClicked()
{
    emit controlBoardClose();
}

void ControlBoard::updateCurrentTurn()
{
    currentTurn_->setPlainText(tr("Current Turn: ") + QString::fromUtf8(gameState_->currentPlayerName().c_str()));
    update();
    return;
}
void ControlBoard::updateStage()
{
    stage_->setPlainText(tr("stage_: ") + gameState_->currentGamePhaseName());
    update();
    return;
}
void ControlBoard::updateMovementLeft()
{
    movementLeft_->setPlainText(tr("Action Left: ") + QString::number(gameState_->getActionsLeft()));
    update();
    return;
}
void ControlBoard::updatePoint(std::vector<int> player_IDs)
{
    for (auto player_id : player_IDs)
    {
        auto playerName = QString::fromUtf8(gameState_->getPlayerName(player_id).c_str());
        auto text = pointTxt_[playerName];
        text->setPlainText(tr("• ") + playerName +
                           tr(": ") + QString::number(gameState_->points_->at(gameState_->getPlayerName(player_id))));
        text->setTextWidth(-1);
    }
    update();
    return;
}

void ControlBoard::updateTop10()
{
    auto top10 = *(gameState_->top10);
    for (auto i=0; i<top10.size(); i++)
    {
        QString player_data = tr("• ") +
                              QString::fromUtf8(top10[i].first.c_str()) +
                              tr(": ") + QString::number(top10[i].second);
        top10Txt_[i]->setPlainText(player_data);
        top10Txt_[i]->setTextWidth(-1);

    }
    update();
}

void ControlBoard::setWheelClick(bool flag)
{
    innerWheel_->setClickAction(flag);
    outterWheel_->setClickAction(flag);
}

void ControlBoard::initializePoints()
// visualize initial points of players
{
    auto pos =POINT_POS*boardScale_;

    for (auto it = (*(gameState_->points_)).begin(); it!= (*(gameState_->points_)).end(); it++)
    {
        QString player_data = tr("• ") +
                QString::fromUtf8(it->first.c_str()) +
                tr(": ") + QString::number(it->second);

        auto text = new QGraphicsTextItem(player_data);
        text->adjustSize();
        text->setPos(pos);
        scene_->addItem(text);
        text->setScale(1.1*boardScale_);
        pos += QPointF(0,25*boardScale_);

        // keep pointers to text items
        pointTxt_[QString::fromUtf8(it->first.c_str())] = text;
    }
}

void ControlBoard::initializeTop10()
// visualize top 10 players
{
    auto pos = TOP10_POS*boardScale_;

    for (unsigned int i=0; i < 10; i++)
    {
        QString player_data = "";
        auto text = new QGraphicsTextItem(player_data);
        text->adjustSize();
        text->setPos(pos);
        scene_->addItem(text);
        text->setScale(1.1*boardScale_);
        pos += QPointF(0,25*boardScale_);

        // keep pointers to text items
        top10Txt_.push_back(text);
    }

    updateTop10();
}

void ControlBoard::initializeInnerWheel()
{
    // load wheel symbol
    innerWheel_ = new UI::Wheel(":/Images/inner_wheel.png", 5, 30, false, this);
    innerWheel_->setScale(0.35*boardScale_);
    scene_->addItem(innerWheel_);

    auto pos = innerWheel_->scenePos();

    connect(innerWheel_, SIGNAL(clicked()), this, SLOT(wheelClicked()));
    connect(this, SIGNAL(animateInnerWheel(int)), innerWheel_, SLOT(spin(int)));

    QPointF target_pos = WHEEL_POS*boardScale_ - innerWheel_->scenePos()*boardScale_;
    innerWheel_->setPos(target_pos);

    pos = innerWheel_->scenePos();
}

void ControlBoard::initializeOutterWheel()
{
    // load wheel symbol
    outterWheel_ = new UI::Wheel(":/Images/outter_wheel.png", 5, 30, true, this);
    outterWheel_->setScale(0.35*boardScale_);
    scene_->addItem(outterWheel_);

    connect(outterWheel_, SIGNAL(clicked()), this, SLOT(wheelClicked()));
    connect(this, SIGNAL(animateOutterWheel(int)), outterWheel_, SLOT(spin(int)));

    auto pos = outterWheel_->scenePos();
    auto origin = outterWheel_->transformOriginPoint();

    QPointF target_pos = WHEEL_POS*boardScale_ - outterWheel_->scenePos()*boardScale_;
    outterWheel_->setPos(target_pos);

    pos = outterWheel_->scenePos();
    origin = outterWheel_->transformOriginPoint();


    // marker_
    QVector<QPointF> vertex;
    QPointF v1 = outterWheel_->pos() + QPointF(410, 300)*boardScale_;

    vertex.push_back(v1);
    vertex.push_back(v1 + QPointF(20, -10)*boardScale_);
    vertex.push_back(v1 + QPointF(20, 10)*boardScale_);

    QPolygonF triagle(vertex);
    marker_ = new QGraphicsPolygonItem(triagle);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::red);
    marker_->setBrush(brush);
    scene_->addItem(marker_);

}
}



