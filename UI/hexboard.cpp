#include "hexboard.h"
#include <QGraphicsTextItem>
#include <hex.hh>
#include <graphichex.h>
#include <graphicpawn.h>
#include <graphicactor.h>
#include <gameengine.hh>
#include <QVector>
#include <QPointF>
#include <QScrollBar>
#include <QTimeLine>
#include <QWheelEvent>
#include <illegalmoveexception.hh>
#include <iostream>
#include <cmath>
#include <cstdlib>

HexBoard::HexBoard(std::shared_ptr<Common::IGameRunner> game_engine_ptr,
                       std::shared_ptr<GameBoard> gameboard_ptr,
                       std::shared_ptr<GameState> gamestate_ptr,
                       std::vector<std::shared_ptr<Common::IPlayer>> game_players,
                       int width,
                       int height,
                       int board_scale,
                       QWidget* parent)
    : QGraphicsView (parent)

{

    this->game_engine = game_engine_ptr;
    this->game_board = gameboard_ptr;
    this->game_state = gamestate_ptr;
    this->players = game_players;
    this->board_scale = board_scale;
    this->width = width;
    this->height = height;

    QVector<QColor> player_colors;
    player_colors.push_back(Qt::red);
    player_colors.push_back(Qt::black);
    player_colors.push_back(Qt::blue);
    player_colors.push_back(Qt::white);
    player_colors.push_back(QColor(128, 0, 128));
    player_colors.push_back(QColor(165, 42, 42));
    player_colors.push_back(QColor(245, 130, 48));
    player_colors.push_back(QColor(255, 250, 200));

    int player_count = 0;
    for (auto player : game_players)
    {
        auto name = QString::fromUtf8(game_state->get_player_name(player->getPlayerId()).c_str());
        pawn_colors[name] = player_colors[player_count];
        player_count += 1;
    }

    // set up the screen
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(width, height);

    h_scrollbar = new QScrollBar(this);
    h_scrollbar->setMinimumHeight(100);

    v_scrollbar = new QScrollBar(this);
    v_scrollbar->setMinimumWidth(100);

    setHorizontalScrollBar(h_scrollbar);
    setVerticalScrollBar(v_scrollbar);

    // set up the scene
    scene = new QGraphicsScene();
    scene->setSceneRect(0,0,160*board_scale,180*board_scale);
    setScene(scene);

    scene->clear();
    populate();
}


void HexBoard::hex_clicked(int id)
// TODO: check when flip result return actor's name -> what's that for?
{
    if (game_state->currentGamePhase() != 2)
    {
        return;
    }

    auto graphic_hex = graphic_hex_list[id];
    auto coord = graphic_hex->get_hex()->getCoordinates();
    try {
        auto flip_result = game_engine->flipTile(coord);

        std::cerr << "finish trying to flip tile \n";
        if (flip_result.compare("") == 0)
        {
            graphic_hex->flip();
        }
    } catch (Common::IllegalMoveException exception) {
        std::cerr << "******EXCEPTION: " << exception.msg() << "\n";
    }
}

void HexBoard::pawn_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos)
{
    // check and perform pawn movement
}

void HexBoard::actor_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos)
{
    // check and perform actor movement
}

void HexBoard::transport_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos)
{
    // check and perform transport movement
}

void HexBoard::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    wheel_schedule += numSteps;
    if (wheel_schedule * numSteps < 0)
    wheel_schedule = numSteps;

    QTimeLine *anim = new QTimeLine(350, this);
    anim->setUpdateInterval(20);

    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    connect(anim, SIGNAL (finished()), SLOT (animFinished()));
    anim->start();
}

void HexBoard::add_pawn(int pawn_id, std::shared_ptr<Common::Pawn> pawn_ptr)
// TODO: add signals and slots related to pawn movement and such
{
    auto hex_coord = pawn_ptr->getCoordinates();
    auto pawn_pos = generate_pawn_position(hex_coord);
    auto owner = game_state->get_player_name(pawn_ptr->getPlayerId());
    auto pawn_color = get_pawn_color(owner);

    QVector<QPointF> pawn_vertex;
    pawn_vertex << QPointF(0,0)*board_scale;
    pawn_vertex << QPointF(1,0)*board_scale;
    pawn_vertex << QPointF(1,1)*board_scale;
    pawn_vertex << QPointF(0,1)*board_scale;

    auto graphic_pawn = new GraphicPawn(pawn_ptr, pawn_vertex, pawn_color, owner);

    scene->addItem(graphic_pawn);
    graphic_pawn->setPos(pawn_pos.first*board_scale, pawn_pos.second*board_scale);
    graphic_pawn_list.insert(pawn_id, graphic_pawn);
    connect(graphic_pawn, SIGNAL(pawn_is_moved(int, QPointF, QPointF)), this, SLOT(pawn_is_moved(int, QPointF, QPointF)));

}

void HexBoard::add_actor(int actor_id, std::shared_ptr<Common::Actor> actor_ptr)
{

}

void HexBoard::add_transport(int transport_id, std::shared_ptr<Common::Transport> transport_ptr)
{

}

void HexBoard::scalingTime(qreal x)
{
    qreal factor = 1.0+ qreal(wheel_schedule) / 300.0;
    this->scale(factor, factor);
}

void HexBoard::animFinished()
{
    if (wheel_schedule > 0)
    wheel_schedule--;
    else
    wheel_schedule++;
    sender()->~QObject();
}

// add hex to scene
void HexBoard::add_hex(std::shared_ptr<Common::Hex> hex_ptr, int id)
{
    // calculate vertex coordinates
    QVector<QPointF> vertex = cube_to_hex_vertex(hex_ptr->getCoordinates());
    if (hex_ptr->getActorTypes().size()>0)
    {
        std::cerr << "actor type: " << hex_ptr->getActorTypes()[0] << "\n";
    }
    if (hex_ptr->getTransports().size()>0)
    {
        std::cerr << "exist transport\n";
    }

    // create graphic_hex object
    GraphicHex* graphic_hex = new GraphicHex(hex_ptr, vertex, id);

    // add graphic_hex to scene
    scene->addItem(graphic_hex);

    // add to list of graphic hex
    graphic_hex_list.insert(id, graphic_hex);

    connect(graphic_hex, SIGNAL(clicked(int)), this, SLOT(hex_clicked(int)));
}

// populate main window based on data from game_board, TODO add pawn and actors part
void HexBoard::populate()
{
    // add hex
    int hex_id = 0;
    for (auto it: *(game_board->getHexList()))
    {
        add_hex(it.second, hex_id);
        hex_id += 1;
    }

    // add pawns
    for (auto it: *(game_board->getPawnList()))
    {
        int pawn_id = it.first;
        auto pawn_ptr = it.second.first;

        std::cerr << "adding pawn \n";
        add_pawn(pawn_id, pawn_ptr);
    }

    GraphicActor* actor = new GraphicActor(nullptr, nullptr);
    scene->addItem(actor);
    actor->setPos(300,300);
}

// convert cube coordinate to 2D coordinate
std::pair<double, double> HexBoard::cube_to_hex_center(Common::CubeCoordinate coord)
{
    double x, y;
    x = 3*coord.x - 3*coord.y + 67;
    y = 5*coord.x + 5*coord.y + 76.5;
    return std::make_pair(x,y);
}

// convert cube coordinate to hex vertices coordinate
QVector<QPointF> HexBoard::cube_to_hex_vertex(Common::CubeCoordinate coord)
{
    auto center = cube_to_hex_center(coord);
    QVector<QPointF> vertex;
    vertex << board_scale*QPointF(center.first, center.second-3.5);
    vertex << board_scale*QPointF(center.first +3, center.second-1.5);
    vertex << board_scale*QPointF(center.first +3, center.second+1.5);
    vertex << board_scale*QPointF(center.first, center.second+3.5);
    vertex << board_scale*QPointF(center.first -3, center.second+1.5);
    vertex << board_scale*QPointF(center.first -3, center.second-1.5);

    return vertex;
}



std::pair<double, double> HexBoard::generate_pawn_position(Common::CubeCoordinate coord)
{
    auto hex_center = cube_to_hex_center(coord);

    QString key = QString::number(coord.x) + QString::number(coord.y);
    // if no key exist -> 1st pawn in the hex
    if (pawn_vertices.find(key) == pawn_vertices.end())
    {
        pawn_vertices[key] = 1;
        auto pawn_center = std::make_pair(hex_center.first-2.5, hex_center.second-0.5);
        return pawn_center;
    }
    else
    {
        int no_existing_pawn = pawn_vertices[key];
        if (no_existing_pawn == 1)
        {
            pawn_vertices[key] += 1;
            auto pawn_center = std::make_pair(hex_center.first-1.5, hex_center.second-2.0);
            return pawn_center;
        }
        else if (no_existing_pawn == 2)
        {
            pawn_vertices[key] += 1;
            auto pawn_center = std::make_pair(hex_center.first+0.5, hex_center.second-2.0);
            return pawn_center;
        }
        else
        {
            pawn_vertices[key] += 1;
            auto pawn_center = std::make_pair(hex_center.first+1.5, hex_center.second-0.5);
            return pawn_center;
        }
    }
}


QColor HexBoard::get_pawn_color(std::string pawn_owner)
{
    return pawn_colors[QString::fromUtf8(pawn_owner.c_str())];
}

