#include <controlboard.h>
#include <QGraphicsProxyWidget>
#include <iostream>
#include <QTimer>
#include <QObject>
#include <QtWidgets>
#include <igamerunner.hh>
#include <iostream>
#include <closingwindow.h>

ControlBoard::~ControlBoard()
{
    // TODO: implement destructor
    return;
}


ControlBoard::ControlBoard(std::shared_ptr<Common::IGameRunner> game_engine,
                           std::shared_ptr<GameState> game_state,
                           double board_scale,
                           QWidget* parent)
    : QGraphicsView (parent)

{
    this->game_engine = game_engine;
    this->game_state = game_state;
    this->allow_quit = false;
    this->board_scale = board_scale;


    setFixedSize(int(WIDTH*board_scale), int(HEIGHT*board_scale));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->scene = new QGraphicsScene(this);
    scene->setSceneRect(0,0,int(WIDTH*board_scale), int(HEIGHT*board_scale));
    setScene(scene);
    scene->clear();

    // handle buttons

    play_button = new QPushButton(tr("Play"));
    play_button->setFixedSize(int(BUTTON_SIZE.first*board_scale), int(BUTTON_SIZE.second*board_scale));
    QGraphicsProxyWidget* play_button_proxy = scene->addWidget(play_button);
    play_button_proxy->setPos(PLAY_BUTTON_POS*board_scale);
    connect(play_button, SIGNAL(clicked()), this, SLOT(deactivate_play_button()));


    save_button = new QPushButton(tr("Save"));
    save_button->setToolTip(tr("Save current game"));
    save_button->setFixedSize(int(BUTTON_SIZE.first*board_scale), int(BUTTON_SIZE.second*board_scale));
    auto save_button_proxy = scene->addWidget(save_button);
    save_button_proxy->setPos(SAVE_BUTTON_POS*board_scale);
    connect(save_button, SIGNAL(clicked()), this, SLOT(save_button_clicked()));


    quit_button = new QPushButton(tr("Quit"));
    quit_button->setFixedSize(int(BUTTON_SIZE.first*board_scale), int(BUTTON_SIZE.second*board_scale));
    auto quit_button_proxy = scene->addWidget(quit_button); // const std::pair<int, int> QUIT_BUTTON_POS = std::make_pair(245,15);
    quit_button_proxy->setPos(QUIT_BUTTON_POS*board_scale);
    connect(quit_button, SIGNAL(clicked()), this, SLOT(quit_button_clicked()));

    // handle game state display
    current_turn = new QGraphicsTextItem(tr("Current Turn: ") + QString::fromUtf8(game_state->currentPlayerName().c_str()));
    current_turn->adjustSize();
    current_turn->setPos(TURN_POS*board_scale);
    scene->addItem(current_turn);
    current_turn->setScale(1.1*board_scale);

    stage = new QGraphicsTextItem(tr("Stage: ") + game_state->currentGamePhaseName());
    stage->adjustSize();
    stage->setPos(STAGE_POS*board_scale);
    scene->addItem(stage);
    stage->setScale(1.1*board_scale);

    movement_left = new QGraphicsTextItem(tr("Action Left: ") + QString::number(game_state->getActionsLeft()));
    movement_left->adjustSize();
    movement_left->setPos(ACTION_POS*board_scale);
    scene->addItem(movement_left);
    movement_left->setScale(1.1*board_scale);

    // handle statistics

    points_title = new QGraphicsTextItem(tr("** POINTS **"));
    points_title->adjustSize();
    points_title->setPos(POINT_TT_POS*board_scale);
    scene->addItem(points_title);
    points_title->setScale(1.25*board_scale);
    initialize_points();

    top10_title = new QGraphicsTextItem(tr("** TOP10 **"));
    top10_title->adjustSize();
    top10_title->setPos(TOP10_TT_POS*board_scale);
    scene->addItem(top10_title);
    top10_title->setScale(1.25*board_scale);
    initialize_top10();

    // handle wheel
    initialize_inner_wheel();
    initialize_outter_wheel();

}

void ControlBoard::save_button_clicked()
// TODO: perform saving function
{
    emit control_board_close();
}

void ControlBoard::wheel_clicked()
{
    auto wheel_output = game_engine->spinWheel();
    int inner_degree, outter_degree;

    if (wheel_output.first.compare("dolphin")==0)
    {
        inner_degree = 0;
    }
    else if (wheel_output.first.compare("kraken")==0) {
        inner_degree = -90;
    }
    else if (wheel_output.first.compare("seamunster")==0) {
        inner_degree = -180;
    }
    else{
        inner_degree = -270;
    }

    if (wheel_output.second.compare("1")==0)
    {
        outter_degree = 270;
    }
    else if (wheel_output.second.compare("2")==0) {
        outter_degree = 0;
    }
    else if (wheel_output.second.compare("3")==0) {
        outter_degree = 90;
    }
    else{
        outter_degree = 180;
    }

    emit animate_inner_wheel(inner_degree);
    emit animate_outter_wheel(outter_degree);
}

void ControlBoard::quit_game()
{
    close();
}

void ControlBoard::quit_button_clicked()
{
    save_button_clicked();
}


void ControlBoard::deactivate_play_button()
{
    play_button->setEnabled(false);
    return;
}

void ControlBoard::update_current_turn()
{
    current_turn->setPlainText(tr("Current Turn: ") + QString::fromUtf8(game_state->currentPlayerName().c_str()));
    update();
    return;
}
void ControlBoard::update_stage()
{
    stage->setPlainText(tr("Stage: ") + game_state->currentGamePhaseName());
    update();
    return;
}
void ControlBoard::update_movement_left()
{
    movement_left->setPlainText(tr("Action Left: ") + QString::number(game_state->getActionsLeft()));
    std::cerr << "receive update signal for movement left: " << game_state->getActionsLeft() << "\n";
    update();
    return;
}
void ControlBoard::update_point(std::vector<int> player_IDs)
{
    for (auto player_id : player_IDs)
    {
        auto player_name = QString::fromUtf8(game_state->get_player_name(player_id).c_str());
        auto text = points_txt[player_name];
        text->setPlainText(tr("• ") + player_name +
                           tr(": ") + QString::number(game_state->points->at(game_state->get_player_name(player_id))));
    }
    update();
    return;
}

void ControlBoard::update_top10()
{
    int index = 0;
    for (auto it : *(game_state->top10))
    {
        QString player_data = tr("• ") +
                              QString::fromUtf8(it.first.c_str()) +
                              tr(": ") + QString::number(it.second);

        top10_txt[index]->setPlainText(player_data);
        index += 1;
    }
    update();
}

void ControlBoard::initialize_points()
// visualize initial points of players
{
    auto pos =POINT_POS*board_scale;

    for (auto it = (*(game_state->points)).begin(); it!= (*(game_state->points)).end(); it++)
    {
        QString player_data = tr("• ") +
                QString::fromUtf8(it->first.c_str()) +
                tr(": ") + QString::number(it->second);

        auto text = new QGraphicsTextItem(player_data);
        text->adjustSize();
        text->setPos(pos);
        scene->addItem(text);
        text->setScale(1.1*board_scale);
        pos += QPointF(0,25*board_scale);

        // keep pointers to text items
        points_txt[QString::fromUtf8(it->first.c_str())] = text;
    }
}

void ControlBoard::initialize_top10()
// visualize top 10 players
{
    auto pos = TOP10_POS*board_scale;

    for (auto it : *(game_state->top10))
    {
        QString player_data = tr("• ") +
                              QString::fromUtf8(it.first.c_str()) +
                              tr(": ") + QString::number(it.second);
        auto text = new QGraphicsTextItem(player_data);
        text->adjustSize();
        text->setPos(pos);
        scene->addItem(text);
        text->setScale(1.1*board_scale);
        pos += QPointF(0,25*board_scale);

        // keep pointers to text items
        top10_txt.push_back(text);
    }
}

void ControlBoard::initialize_inner_wheel()
{
    // load wheel symbol
    inner_wheel = new Wheel(":/Images/inner_wheel.png", 5, 30, false, this);
    inner_wheel->setScale(0.35*board_scale);
    scene->addItem(inner_wheel);

    auto pos = inner_wheel->scenePos();
    std::cerr << "inner wheel scene pos: " << pos.rx() << ", " << pos.ry() << "\n";

    connect(inner_wheel, SIGNAL(clicked()), this, SLOT(wheel_clicked()));
    connect(this, SIGNAL(animate_inner_wheel(int)), inner_wheel, SLOT(spin(int)));

    QPointF target_pos = WHEEL_POS*board_scale - inner_wheel->scenePos()*board_scale;
    inner_wheel->setPos(target_pos);

    pos = inner_wheel->scenePos();
    std::cerr << "inner wheel scene pos: " << pos.rx() << ", " << pos.ry() << "\n";

//    auto rect = inner_wheel->boundingRect();
//    std::cerr << "inner wheel bbox: " << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << "\n";
}

void ControlBoard::initialize_outter_wheel()
{
    // load wheel symbol
    outter_wheel = new Wheel(":/Images/outter_wheel.png", 5, 30, true, this);
    outter_wheel->setScale(0.35*board_scale);
    scene->addItem(outter_wheel);

    connect(outter_wheel, SIGNAL(clicked()), this, SLOT(wheel_clicked()));
    connect(this, SIGNAL(animate_outter_wheel(int)), outter_wheel, SLOT(spin(int)));

    auto pos = outter_wheel->scenePos();
    auto origin = outter_wheel->transformOriginPoint();
    std::cerr << "outter wheel scene pos: " << pos.rx() << ", " << pos.ry() << "\n";
    std::cerr << "transform origin: " << origin.rx() << ", " << origin.ry() << "\n";

    QPointF target_pos = WHEEL_POS*board_scale - outter_wheel->scenePos()*board_scale;
    outter_wheel->setPos(target_pos);

    pos = outter_wheel->scenePos();
    origin = outter_wheel->transformOriginPoint();
    std::cerr << "outter wheel scene pos: " << pos.rx() << ", " << pos.ry() << "\n";
    std::cerr << "transform origin: " << origin.rx() << ", " << origin.ry() << "\n";


    // marker
    QVector<QPointF> vertex;
    QPointF v1 = outter_wheel->pos() + QPointF(410, 300)*board_scale;

    vertex.push_back(v1);
    vertex.push_back(v1 + QPointF(20, -10)*board_scale);
    vertex.push_back(v1 + QPointF(20, 10)*board_scale);

    QPolygonF triagle(vertex);
    marker = new QGraphicsPolygonItem(triagle);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::red);
    marker->setBrush(brush);
    scene->addItem(marker);

}





