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
#include <list>
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
    std::cerr << "pawn is moved\n";

    auto old_cube_pos = plane_to_cube(old_pos.rx()/board_scale+0.5, old_pos.ry()/board_scale+0.5);
    auto new_cube_pos = plane_to_cube(new_pos.rx()/board_scale+0.5, new_pos.ry()/board_scale+0.5);

    std::cerr << "old cube coord: " << old_cube_pos.x << ", " << old_cube_pos.y << " \n";
    std::cerr << "new cube coord: " << new_cube_pos.x << ", " << new_cube_pos.y << " \n";

    int move_left = game_engine->checkPawnMovement(old_cube_pos, new_cube_pos, pawn_id);
    if (move_left == -1)
    {
        std::cerr << "pawn movement is forbidden \n";
        graphic_pawn_list[pawn_id]->setPos(old_pos);
        update();
    }
    else
    {
        std::cerr << "move left: " << move_left << "\n";
        game_engine->movePawn(old_cube_pos, new_cube_pos, pawn_id);
        game_state->setActionsLeft(move_left);

        emit(change_movement_left());
        if (move_left == 0)
        {
            game_state->changeGamePhase(Common::GamePhase(2));
            emit(set_pawn_movement(false, current_player_pawn_list));
            current_player_pawn_list.clear();
            emit(change_stage());
        }
    }
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
    connect(this, SIGNAL(set_pawn_movement(bool, std::list<int>)), graphic_pawn, SLOT(allow_movement(bool, std::list<int>)));
}

void HexBoard::add_actor(int actor_id, std::shared_ptr<Common::Actor> actor_ptr)
{

}

void HexBoard::add_transport(int transport_id, std::shared_ptr<Common::Transport> transport_ptr)
{
    auto hex_coord = transport_ptr->getHex()->getCoordinates();
    auto transport_pos = cube_to_hex_center(hex_coord);

    auto graphic_transport = new GraphicTransport(transport_ptr);
    scene->addItem(graphic_transport);
    graphic_transport->setPos(board_scale*transport_pos.first, board_scale*transport_pos.second);

    graphic_transport_list.insert(transport_id, graphic_transport);

    connect(graphic_transport, SIGNAL(transport_is_moved(int, QPointF, QPointF)), this, SLOT(transport_is_moved(int, QPointF, QPointF)));
    connect(this, SIGNAL(set_transport_movement(bool)), graphic_transport, SLOT(allow_movement(bool)));

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
    QPointF pos = cube_to_hex_pos(hex_ptr->getCoordinates());

    QVector<QPointF> vertices;
    vertices << QPointF(3.0, 0.0)*board_scale;
    vertices << QPointF(6.0, 2.0)*board_scale;
    vertices << QPointF(6.0, 5.0)*board_scale;
    vertices << QPointF(3.0, 7.0)*board_scale;
    vertices << QPointF(0.0, 5.0)*board_scale;
    vertices << QPointF(0.0, 2.0)*board_scale;

    // create graphic_hex object
    GraphicHex* graphic_hex = new GraphicHex(hex_ptr, vertices, id);

    // add graphic_hex to scene
    scene->addItem(graphic_hex);
    graphic_hex->setPos(pos);

    // add to list of graphic hex
    graphic_hex_list.insert(id, graphic_hex);

    connect(graphic_hex, SIGNAL(clicked(int)), this, SLOT(hex_clicked(int)));
}

// populate main window based on data from game_board, TODO add boat
void HexBoard::populate()
{
    // add hex
    int hex_id = 0;
    for (auto it: *(game_board->getHexList()))
    {
        add_hex(it.second, hex_id);
        hex_id += 1;
    }

    // add boats
    for (auto it : *(game_board->getTransportList()))
    {
        int transport_id = it.first;
        auto transport_ptr = it.second.first;
        add_transport(transport_id, transport_ptr);
    }

    // add pawns
    for (auto it: *(game_board->getPawnList()))
    {
        int pawn_id = it.first;
        auto pawn_ptr = it.second.first;
        add_pawn(pawn_id, pawn_ptr);
    }

    // activate pawns of current player
    for (auto pawn_it=graphic_pawn_list.begin(); pawn_it!=graphic_pawn_list.end(); pawn_it++)
    {
        if (pawn_it.value()->get_pawn()->getPlayerId() == game_state->currentPlayer())
        {
            current_player_pawn_list.push_back(pawn_it.key());
        }
    }

    emit(set_pawn_movement(true, current_player_pawn_list));
}

double HexBoard::euclidean_distance(std::pair<double, double> x, std::pair<double, double> y)
{
    return std::pow(x.first - y.first, 2) + std::pow(x.second - y.second, 2);
}

// convert cube coordinate to 2D coordinate
std::pair<double, double> HexBoard::cube_to_hex_center(Common::CubeCoordinate coord)
{
    double x, y;
    x = 3*coord.x - 3*coord.y + 67;
    y = 5*coord.x + 5*coord.y + 76.5;
    return std::make_pair(x,y);
}

Common::CubeCoordinate HexBoard::plane_to_cube(double x, double y)
{
    Common::CubeCoordinate output_pos;
    double cy = (y - x - 9.5)/8.0;
    double cx = y/5.0 - cy - 15.3;

    std::vector<Common::CubeCoordinate> cubes;
    std::vector<std::pair<double, double>> centers;

    cubes.push_back(Common::CubeCoordinate(std::floor(cx), std::floor(cy), -std::floor(cx) -std::floor(cy)));
    cubes.push_back(Common::CubeCoordinate(std::floor(cx), std::ceil(cy), -std::floor(cx) -std::ceil(cy)));
    cubes.push_back(Common::CubeCoordinate(std::ceil(cx), std::floor(cy), -std::ceil(cx) -std::floor(cy)));
    cubes.push_back(Common::CubeCoordinate(std::ceil(cx), std::ceil(cy), -std::ceil(cx) -std::ceil(cy)));

    for (auto cube : cubes)
    {
        centers.push_back(cube_to_hex_center(cube));
    }

    auto pos = std::make_pair(x,y);
    double min_dis = std::numeric_limits<double>::max();


    for (unsigned int i=0; i<4; i++)
    {
        auto dist = euclidean_distance(pos, centers[i]);
        if (dist < min_dis)
        {
            min_dis = dist;
            output_pos = cubes[i];
        }
    }

    return output_pos;
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

QPointF HexBoard::cube_to_hex_pos(Common::CubeCoordinate coord)
{
    auto center = cube_to_hex_center(coord);
    QPointF pos = QPointF((center.first - 3.0)*board_scale, (center.second-3.5)*board_scale);
    return pos;
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

