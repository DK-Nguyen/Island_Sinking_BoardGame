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

void ControlBoard::closeEvent(QCloseEvent *event)
{
    if (not allow_quit)
    {
        event->ignore();
        CloseWindow* close_window = new CloseWindow();
        connect(close_window->save_button, SIGNAL(clicked()), this, SLOT(save_button_clicked()));
        connect(close_window->quit_button, SIGNAL(clicked()), this, SLOT(quit_game()));
        close_window->show();
    }
    else{
        event->accept();
    }
}

ControlBoard::ControlBoard(std::shared_ptr<Common::IGameRunner> game_engine,
                           QSharedPointer<QHash<QString, int>> current_points,
                           QSharedPointer<QHash<QString, int>> top10,
                           QWidget* parent)
    : QGraphicsView (parent)

{

    this->config = config;
    this->points = current_points;
    this->top10 = top10;
    this->game_engine = game_engine;
    this->allow_quit = false;

    setFixedSize(350, 720);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->scene = new QGraphicsScene();
    scene->setSceneRect(0,0,350,720);
    setScene(scene);
    scene->clear();

    // handle buttons

    play_button = new QPushButton(tr("Play"));
    play_button->setFixedSize(70, 35);
    QGraphicsProxyWidget* play_button_proxy = scene->addWidget(play_button);
    play_button_proxy->setPos(35, 15);
    connect(play_button, SIGNAL(clicked()), this, SLOT(deactivate_play_button()));


    save_button = new QPushButton(tr("Save"));
    save_button->setToolTip(tr("Save current game"));
    save_button->setFixedSize(70, 35);
    auto save_button_proxy = scene->addWidget(save_button);
    save_button_proxy->setPos(140, 15);
    connect(save_button, SIGNAL(clicked()), this, SLOT(save_button_clicked()));


    quit_button = new QPushButton(tr("Quit"));
    quit_button->setFixedSize(70, 35);
    auto quit_button_proxy = scene->addWidget(quit_button);
    quit_button_proxy->setPos(245, 15);
    connect(quit_button, SIGNAL(clicked()), this, SLOT(quit_button_clicked()));

    // handle game state display
    current_turn = new QGraphicsTextItem(tr("Current Turn: Player 1"));
    current_turn->adjustSize();
    current_turn->setPos(95, 70);
    scene->addItem(current_turn);
    current_turn->setScale(1.1);

    stage = new QGraphicsTextItem(tr("Stage: movement"));
    stage->adjustSize();
    stage->setPos(105, 100);
    scene->addItem(stage);
    stage->setScale(1.1);

    movement_left = new QGraphicsTextItem(tr("Move Left: 3"));
    movement_left->adjustSize();
    movement_left->setPos(125, 130);
    scene->addItem(movement_left);
    movement_left->setScale(1.1);

    // handle statistics

    points_title = new QGraphicsTextItem(tr("** POINTS **"));
    points_title->adjustSize();
    points_title->setPos(15, 170);
    scene->addItem(points_title);
    points_title->setScale(1.25);
    initialize_points();

    top10_title = new QGraphicsTextItem(tr("** TOP10 **"));
    top10_title->adjustSize();
    top10_title->setPos(190, 170);
    scene->addItem(top10_title);
    top10_title->setScale(1.25);
    show_top10();

    // handle wheel
    initialize_inner_wheel();
    initialize_outter_wheel();

}



void ControlBoard::save_button_clicked()
// TODO: perform saving function
{
    // perform saving function


    emit control_board_close();
    quit_game();
    return;
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
    // emit control_board_close signal
    allow_quit = true;
    emit control_board_close();
    close();
}

void ControlBoard::quit_button_clicked()
{
    QCloseEvent * event = new QCloseEvent();
    closeEvent(event);
}


void ControlBoard::deactivate_play_button()
{
    play_button->setEnabled(false);
    return;
}

void ControlBoard::update_current_turn(QString current_player)
{
    return;
}
void ControlBoard::update_stage(QString current_stage)
{
    return;
}
void ControlBoard::update_movement_left(int movement_left)
{
    return;
}
void ControlBoard::update_point(QStringList)
{
    return;
}

void ControlBoard::initialize_points()
// visualize initial points of players
{
    int x = 15;
    int y = 200;

    for (auto it = (*points).begin(); it!= (*points).end(); it++)
    {
        QString player_data = tr("• ") + it.key() + tr(": ") + QString::number(it.value());
        auto text = new QGraphicsTextItem(player_data);
        text->adjustSize();
        text->setPos(x, y);
        scene->addItem(text);
        text->setScale(1.1);
        y += 25;

        // keep pointers to text items
        points_txt[it.key()] = text;
    }
}

void ControlBoard::show_top10()
// visualize top 10 players
{
    int x = 190;
    int y = 200;

    for (auto it = (*top10).begin(); it!= (*top10).end(); it++)
    {
        QString player_data = tr("• ") + it.key() + tr(": ") + QString::number(it.value());
        auto text = new QGraphicsTextItem(player_data);
        text->adjustSize();
        text->setPos(x, y);
        scene->addItem(text);
        text->setScale(1.1);
        y += 25;
    }
}

void ControlBoard::initialize_inner_wheel()
{
    // load wheel symbol
    inner_wheel = new Wheel(":/Images/inner_wheel.png", 5, 30, false, this);
    inner_wheel->setScale(0.35);
    scene->addItem(inner_wheel);

    QPointF target_pos = QPointF(40, 500) - inner_wheel->scenePos();
    inner_wheel->setPos(target_pos);

    connect(inner_wheel, SIGNAL(clicked()), this, SLOT(wheel_clicked()));
    connect(this, SIGNAL(animate_inner_wheel(int)), inner_wheel, SLOT(spin(int)));
}

void ControlBoard::initialize_outter_wheel()
{
    // load wheel symbol
    outter_wheel = new Wheel(":/Images/outter_wheel.png", 5, 30, true, this);
    outter_wheel->setScale(0.35);
    scene->addItem(outter_wheel);

    QPointF target_pos = QPointF(40, 500) - outter_wheel->scenePos();
    outter_wheel->setPos(target_pos);

    // marker
    QVector<QPointF> vertex;
    QPointF v1 = outter_wheel->pos() + QPointF(410, 300);

    std::cerr << v1.rx() << ", " << v1.ry() << "\n";

    vertex.push_back(v1);
    vertex.push_back(v1 + QPointF(20, -10));
    vertex.push_back(v1 + QPointF(20, 10));

    QPolygonF triagle(vertex);
    marker = new QGraphicsPolygonItem(triagle);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::red);
    marker->setBrush(brush);
    scene->addItem(marker);

    connect(outter_wheel, SIGNAL(clicked()), this, SLOT(wheel_clicked()));
    connect(this, SIGNAL(animate_outter_wheel(int)), outter_wheel, SLOT(spin(int)));
}





