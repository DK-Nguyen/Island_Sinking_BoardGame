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
#include <boat.hh>
#include <dolphin.hh>
#include <vortex.hh>
#include <seamunster.hh>
#include <shark.hh>
#include <kraken.hh>

HexBoard::HexBoard(std::shared_ptr<Common::IGameRunner> game_engine_ptr,
                       std::shared_ptr<Student::GameBoard> gameboard_ptr,
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
    this->current_actor_id = 100;
    this->current_transport_id = 500;

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

void HexBoard::clear()
// clear all data on HexBoard
{

}


void HexBoard::hex_clicked(int id)
/***
LOGIC
-----

if not during stage 2: ignore
if stage 2:
    - if can flip:
        - change color of graphic hex
        - create new graphic actor/transport and add to backend, graphic
        - if vortex:
            - do_vortex_action
        - change stage to 3, update controlboard
        - enable wheel click


TODO
----
- logic modified, need to change implementation
- verify logic versus implementation

***/
{
    if (game_state->currentGamePhase() != 2)
    {
        return;
    }

    auto graphic_hex = graphic_hex_list[id];
    auto coord = graphic_hex->get_hex()->getCoordinates();
    try {
        auto flip_result = game_engine->flipTile(coord);

        // fail the flip
        if (flip_result.compare("") == 0)
        {
            std::cerr << "failed to flip the tile \n";
        }

        // actor or transport is spawn
        else
        {
            // change color of graphic hex
            graphic_hex_list[id]->flip();

            // create new actor/transport
            if (flip_result.compare("boat")==0)
            {
                auto boat = std::make_shared<Common::Boat>(current_transport_id);
                game_board->addTransport(boat, graphic_hex->get_hex()->getCoordinates());
                add_transport(current_transport_id, boat);
                current_transport_id++;
            }
            else if (flip_result.compare("dolphin")==0)
            {
                auto dolphin = std::make_shared<Common::Dolphin>(current_transport_id);
                game_board->addTransport(dolphin, graphic_hex->get_hex()->getCoordinates());
                add_transport(current_transport_id, dolphin);
                current_transport_id++;
            }

            else if (flip_result.compare("kraken")==0)
            {
                auto kraken = std::make_shared<Common::Kraken>(current_actor_id);
                game_board->addActor(kraken, graphic_hex->get_hex()->getCoordinates());
                add_actor(current_actor_id, kraken);
                current_actor_id++;
            }

            else if (flip_result.compare("vortex")==0)
            {
                // create vortex
                auto vortex = std::make_shared<Common::Vortex>(current_actor_id);
                game_board->addActor(vortex, graphic_hex->get_hex()->getCoordinates());
                add_actor(current_actor_id, vortex);


                // perform action
                do_vortex_action(current_actor_id);

                // change game stage, change player's turn
                game_state->changeGamePhase(Common::GamePhase(1));
                emit(update_stage());

                // change player turn
                change_player_turn(game_state->currentPlayer());

                // enable current player's pawns movement
                enable_pawn_movement();

                current_actor_id++;

            }

            else if (flip_result.compare("shark")==0)
            {
                auto shark = std::make_shared<Common::Shark>(current_actor_id);
                game_board->addActor(shark, graphic_hex->get_hex()->getCoordinates());
                add_actor(current_actor_id, shark);
                current_actor_id++;
            }

            else if (flip_result.compare("seamunster")==0)
            {
                auto seamunster = std::make_shared<Common::Seamunster>(current_actor_id);
                game_board->addActor(seamunster, graphic_hex->get_hex()->getCoordinates());
                add_actor(current_actor_id, seamunster);
                current_actor_id++;
            }

            else
            {
                std::cerr << "the return actor/transport does not match! \n";
                std::cerr << flip_result << "\n";
                exit(0);
            }

            // if not vortex, change to stage 3, enable wheel click
            if (flip_result.compare("vortex")!=0)
            {
                emit set_wheel_click(true);
                game_state->changeGamePhase(Common::GamePhase(3));
                emit update_stage();
            }
        }
    } catch (Common::IllegalMoveException exception) {
        std::cerr << "******EXCEPTION: " << exception.msg() << "\n";
    }
}

void HexBoard::wheel_clicked(std::pair<std::string, std::string> wheel_output)
/***
LOGIC
-----
This slot is called when wheel in controlboard is clicked

set wheel_output_
if wheel output indicates dolphin:
    - enable movement of all dolphins
    - enable pawn movement on the same hexes

if wheel output is actor:
    - enable actor movement


TODO
----

verify logic versus implementation

***/
{
    wheel_output_ = wheel_output;
    // if dolphin, allow movement of pawn and dolphin
    if (wheel_output.first.compare("dolphin")==0)
    {
        // set graphic dolphin movable, also set current player's pawn on that hex movable
        for (auto transport : graphic_transport_list)
        {
            if (transport->get_transport()->getTransportType().compare("dolphin")==0)
            {
                // set dolphin movable
                transport->allow_movement(true);
                auto coord = transport->get_transport()->getHex()->getCoordinates();
                auto key = cube_to_string(coord);

                for (auto pawn : data_map.at(key).pawns)
                {
                    if (pawn->get_pawn()->getPlayerId() == game_state->currentPlayer())
                    {
                        current_player_pawn_list.push_back(pawn->get_pawn()->getId());
                    }
                }
            }
        }

        // set pawn movable
        if (current_player_pawn_list.size() > 0)
        {
            emit set_pawn_movement(true, current_player_pawn_list);
        }
    }

    // if other actors, set movable
    else
    {
        std::list<int> IDs;
        for (auto actor : graphic_actor_list)
        {
            if (actor->get_actor()->getActorType().compare(wheel_output.first) == 0)
            {
                IDs.push_back(actor->get_actor()->getId());
            }
        }

        if (IDs.size() > 0)
        {
            emit set_actor_movement(true, IDs);
        }
    }
}

void HexBoard::pawn_is_moved(int pawn_id, QPointF old_pos, QPointF new_pos)
/***
LOGIC
-----

if pawn is moved within same hex
    - if pawn is moved onto transport
        - if transport is full: move back pawn to old position
        - if transport is not full:
            - add graphic pawn to graphic transport
            - add pawn to transport
    - if pawn is moved out of transport
        - remove graphic pawn from graphic transport
        - remove pawn from transport

if pawn is moved to new hex:
    - if stage 3:
        - move back pawn to old position (stage 3 only allows moving pawn to dolphin)
    - if stage 1:
        - if pawn movement is not valid:
            - move back pawn to old position
        - if pawn movement is valid:
            - move pawn in backend
            - update move left in backend, controlboard
            - if no move left:
                - if island is sunk:
                    - set to stage 3
                    - enable wheel click
                - if island is not sunk:
                    - set to stage 2
                    - enable hex click
                - disable pawn movement
                - clear current_player_pawn_list
                - update stage in controlboard


 TODO:
 - handle moving out of transport
 - handling transitioning between stage
 - need to check actual implementation versus logic description

 * ***/
{
    std::cerr << "pawn is moved\n";

    // get pawn 2D position
    auto old_cube_pos = plane_to_cube(old_pos.rx()/board_scale+0.5, old_pos.ry()/board_scale+0.5);
    auto new_cube_pos = plane_to_cube(new_pos.rx()/board_scale+0.5, new_pos.ry()/board_scale+0.5);

    // moving within the same hex
    if (old_cube_pos == new_cube_pos)
    {
        for (auto it : data_map[cube_to_string(old_cube_pos)].transports)
        {
            // if pawn is moved from outside to transport
            if (is_pawn_on_transport(new_pos, it) && !is_pawn_on_transport(old_pos, it))
            {
                // full transport, move back to old pos
                if (it->is_full())
                {
                    graphic_pawn_list[pawn_id]->setPos(old_pos);
                }

                // if not full, add pawn to graphic transport and backend transport
                else
                {
                    it->add_pawn(graphic_pawn_list[pawn_id]);
                    it->get_transport()->addPawn(graphic_pawn_list[pawn_id]->get_pawn());
                }
            }

            // if pawn is moved out of transport
            if (is_pawn_on_transport(old_pos, it) && !is_pawn_on_transport(new_pos, it))
            {
                // put to new position
                graphic_pawn_list[pawn_id]->setPos(new_pos);

                // remove from graphical transport
                it->remove_pawn(graphic_pawn_list[pawn_id]);

                // remove from backend
                it->get_transport()->removePawn(graphic_pawn_list[pawn_id]->get_pawn());
            }
        }
    }

    // if pawn is moved to another hex
    else
    {
        int move_left = game_engine->checkPawnMovement(old_cube_pos, new_cube_pos, pawn_id);

        // if move is not valid
        if (move_left == -1)
        {
            graphic_pawn_list[pawn_id]->setPos(old_pos);
            update();
        }

        // if move is valid
        else
        {
            // move in backend
            game_engine->movePawn(old_cube_pos, new_cube_pos, pawn_id);
            game_state->setActionsLeft(move_left);

            // emit change info in ControlBoard
            emit(update_movement_left());

            // if no move left afterwards
            if (move_left == 0)
            {
                // if island completely sunk
                if (game_board->isIslandSunk())
                {
                    game_state->changeGamePhase(Common::GamePhase(3));
                }

                // if not sunk
                else
                {
                    game_state->changeGamePhase(Common::GamePhase(2));
                }

                disable_pawn_movement();
                emit(update_stage());
            }
        }
    }
}

void HexBoard::actor_is_moved(int actor_id, QPointF old_pos, QPointF new_pos)
/***
LOGIC
-----


TODO
----
- write logic
- write implementation
- verify logic versus implementation

***/
{
    // check and perform actor movement
}

void HexBoard::transport_is_moved(int transport_id, QPointF old_pos, QPointF new_pos)
/***
LOGIC
-----
stage 1: both dolphin and boat can move
stage 3: only dolphin can move

- if move is not valid:
    - return transport to old position
- if move is valid:
    - should return #move left
    - move graphic pawns on transport with direction new_pos - old_pos
    - move backend pawns to new hex (should not use game_engine->movePawn() because this one check pawn movement validity, should use game_board->movePawn()
    - call update to move left on controlboard

- if dolphin:
    - if neighboring sea tiles has vortex:
        - call do_vortex_action()
- if boat:
    - if new hex has seamunster:
        - call do_seamunster_action()
    - if new hex has kraken:
        - call do_kraken_action()
    - if new hex has shark:
        - call do_shark_action()

- if stage 1:
    - if no move left:
        - if island is sunk: set to stage 3, update stage in controlboard, enable wheel click
        - if island is not sunk: set to stage 2, enable hex click
- if stage 3:
    - set to stage 1, update stage in controlboard
    - set to next player, update player turn in controlboard
    - add pawn_id of next player to current_player_pawn_list -> enable pawn movement
    - clear wheel_output

TODO
----
- implement when new features from course side come
- verify implementation versus logic

***/

{
    // get corresponding cube position
    auto old_cube_pos = plane_to_cube(old_pos.rx()/board_scale + 3.0, old_pos.ry()/board_scale + 5.0);
    auto new_cube_pos = plane_to_cube(new_pos.rx()/board_scale + 3.0, new_pos.ry()/board_scale + 5.0);

    // if still in the same tile, move back
    if (old_cube_pos == new_cube_pos)
    {
        graphic_transport_list[transport_id]->setPos(old_pos);
    }

    // if move to new tile
    else
    {

    }

}



void HexBoard::add_pawn(int pawn_id, std::shared_ptr<Common::Pawn> pawn_ptr)
// add graphic pawn
{
    auto hex_coord = pawn_ptr->getCoordinates();
    auto pawn_pos = generate_pawn_position(hex_coord);
    auto owner = game_state->get_player_name(pawn_ptr->getPlayerId());
    auto pawn_color = get_pawn_color(owner);

    auto graphic_pawn = new GraphicPawn(pawn_ptr, board_scale, pawn_color, owner);

    scene->addItem(graphic_pawn);
    graphic_pawn->setPos(pawn_pos.first*board_scale, pawn_pos.second*board_scale);

    // add data using coordinate
    auto map_key = cube_to_string(hex_coord);
    if (data_map.find(map_key) == data_map.end())
    {
        HexData data;
        data.coord = hex_coord;
        data.pawns.push_back(graphic_pawn);
        data_map.insert(std::make_pair(map_key, data));
    }
    else
    {
        data_map[map_key].pawns.push_back(graphic_pawn);
    }

    graphic_pawn_list.insert(pawn_id, graphic_pawn);

    // connect signals
    connect(graphic_pawn, SIGNAL(pawn_is_moved(int, QPointF, QPointF)), this, SLOT(pawn_is_moved(int, QPointF, QPointF)));
    connect(this, SIGNAL(set_pawn_movement(bool, std::list<int>)), graphic_pawn, SLOT(allow_movement(bool, std::list<int>)));
}

void HexBoard::add_actor(int actor_id, std::shared_ptr<Common::Actor> actor_ptr)
// add graphic actor
{
    auto hex_coord = actor_ptr->getHex()->getCoordinates();
    auto actor_pos = cube_to_hex_pos(hex_coord);

    auto graphic_actor = new GraphicActor(actor_ptr);
    scene->addItem(graphic_actor);

    actor_pos.setX(actor_pos.rx() + 2.0);
    actor_pos.setY(actor_pos.ry() + 3.0);

    graphic_actor->setPos(actor_pos);

    // add data using coordinate
    auto map_key = cube_to_string(hex_coord);
    if (data_map.find(map_key) == data_map.end())
    {
        HexData data;
        data.coord = hex_coord;
        data.actors.push_back(graphic_actor);
        data_map.insert(std::make_pair(map_key, data));
    }
    else
    {
        data_map[map_key].actors.push_back(graphic_actor);
    }

    graphic_actor_list.insert(actor_id, graphic_actor);

    // connect signal
    connect(graphic_actor, SIGNAL(actor_is_moved(int, QPointF, QPointF)), this, SLOT(actor_is_moved(int, QPointF, QPointF)));
    connect(this, SIGNAL(set_actor_movement(bool, std::list<int>)), graphic_actor, SLOT(allow_movement(bool, std::list<int>)));
}

void HexBoard::add_transport(int transport_id, std::shared_ptr<Common::Transport> transport_ptr)
// add graphic transport
{
    auto hex_coord = transport_ptr->getHex()->getCoordinates();
    auto transport_pos = cube_to_hex_pos(hex_coord);

    auto graphic_transport = new GraphicTransport(transport_ptr, board_scale);
    scene->addItem(graphic_transport);
    graphic_transport->setPos(transport_pos);

    // add data using coorindate
    auto map_key = cube_to_string(hex_coord);
    if (data_map.find(map_key) == data_map.end())
    {
        HexData data;
        data.coord = hex_coord;
        data.transports.push_back(graphic_transport);
        data_map.insert(std::make_pair(map_key, data));
    }
    else
    {
        data_map[map_key].transports.push_back(graphic_transport);
    }

    graphic_transport_list.insert(transport_id, graphic_transport);


    connect(graphic_transport, SIGNAL(transport_is_moved(int, QPointF, QPointF)), this, SLOT(transport_is_moved(int, QPointF, QPointF)));
    connect(this, SIGNAL(set_transport_movement(bool)), graphic_transport, SLOT(allow_movement(bool)));
}


void HexBoard::add_hex(std::shared_ptr<Common::Hex> hex_ptr, int id)
// add graphic hex to scene
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


void HexBoard::populate()
// populate graphic items on hexboard
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

    enable_pawn_movement();
}

double HexBoard::euclidean_distance(std::pair<double, double> x, std::pair<double, double> y)
// calculate square euclidean distance
{
    return std::pow(x.first - y.first, 2) + std::pow(x.second - y.second, 2);
}


std::pair<double, double> HexBoard::cube_to_hex_center(Common::CubeCoordinate coord)
// convert cube coordinate to 2D coordinate of hex center, without scaling factor
{
    double x, y;
    x = 3*coord.x - 3*coord.y + 67;
    y = 5*coord.x + 5*coord.y + 76.5;
    return std::make_pair(x,y);
}

Common::CubeCoordinate HexBoard::plane_to_cube(double x, double y)
// convert 2D coordinate to cube coorindate
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

QPointF HexBoard::cube_to_hex_pos(Common::CubeCoordinate coord)
// calculate hex coorindate on scene based on cube coordinate
{
    auto center = cube_to_hex_center(coord);
    QPointF pos = QPointF((center.first - 3.0)*board_scale, (center.second-3.5)*board_scale);
    return pos;
}


std::pair<double, double> HexBoard::generate_pawn_position(Common::CubeCoordinate coord)
// generate initial pawn position on mountain tile
// maximum 4 pawns are put on 1 mountain tile
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
// get pawn color according to owner
{
    return pawn_colors[QString::fromUtf8(pawn_owner.c_str())];
}

bool HexBoard::is_pawn_on_transport(QPointF pawn_pos, GraphicTransport *transport)
// check if pawn is on transport
{
    double pawn_center_x = pawn_pos.rx()/board_scale + 0.5;
    double pawn_center_y = pawn_pos.ry()/board_scale + 0.5;

    if (transport->get_transport()->getTransportType().compare("boat")==0)
    {
        double x_min = transport->scenePos().rx()/board_scale + 1.0;
        double x_max = transport->scenePos().rx()/board_scale + 5.0;
        double y_min = transport->scenePos().ry()/board_scale + 3.5;
        double y_max = transport->scenePos().ry()/board_scale + 5.0;

        if (pawn_center_x > x_min && pawn_center_x < x_max && pawn_center_y > y_min && pawn_center_y < y_max)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        double x_min = transport->scenePos().rx()/board_scale + 2.5;
        double x_max = transport->scenePos().rx()/board_scale + 4.5;
        double y_min = transport->scenePos().ry()/board_scale + 4.0;
        double y_max = transport->scenePos().ry()/board_scale + 5.5;

        if (pawn_center_x > x_min && pawn_center_x < x_max && pawn_center_y > y_min && pawn_center_y < y_max)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void HexBoard::change_player_turn(int player_id)
// change the turn to next player, given the current player's id
{
    // find player index
    unsigned int player_index=0;
    for (unsigned int i=0; i< players.size(); i++)
    {
        if (players[i]->getPlayerId() == player_id)
        {
            player_index = i;
            break;
        }
    }

    // change to index of next player
    if (player_index == players.size()-1)
    {
        player_index = 0;
    }
    else
    {
        player_index += 1;
    }

    game_state->changePlayerTurn(players[player_index]->getPlayerId());
    emit update_player_turn();
}

void HexBoard::update_existing_player()
// update list of survived players
{
    std::list<unsigned int> removed_players;

    // find player who has no pawn left
    for (unsigned int i=0; i < players.size(); i++)
    {
        auto player = players[i];
        int pawn_left = 0;
        for (auto pawn : *(game_board->getPawnList()))
        {
            if (player->getPlayerId() == pawn.second.first->getPlayerId())
            {
                pawn_left++;
            }
        }
        if (pawn_left == 0)
        {
            removed_players.push_back(i);
        }
    }

    // remove player from vector of players
    for (auto index : removed_players)
    {
        players.erase(players.begin() + index);
    }

    // if only 1 player left, clear data and emit game over
    if (players.size() <= 1)
    {
        clear();
        emit game_over();
    }
}

void HexBoard::do_vortex_action(int id)
// destroy everything on current hex and neighboring hexes
{
    HexData data;
    auto graphic_vortex = graphic_actor_list[id];
    auto hex = graphic_vortex->get_actor()->getHex();
    auto neighbors = hex->getNeighbourVector();

    // destroy everything on current hex
    data = data_map[cube_to_string(hex->getCoordinates())];
    remove_data(data, true, true, true);

    // destroy everything on neighboring hexes
    for (auto neighbor_hex : neighbors)
    {
        data = data_map[cube_to_string(neighbor_hex)];
        remove_data(data, true, true, true);
    }
}

void HexBoard::do_kraken_action(int id)
{

}

void HexBoard::do_shark_action(int id)
{

}

void HexBoard::do_seamunster_action(int id)
{

}

std::string HexBoard::cube_to_string(Common::CubeCoordinate coord)
// convert cube coordinate to string as key to hash table
{
    return std::to_string(coord.x) + "+" + std::to_string(coord.y) + "+" + std::to_string(coord.z);
}

void HexBoard::remove_data(HexData &data, bool pawn, bool transport, bool actor)
// perform both graphic and backend data remove with given flags
{
    // remove pawns
    if (pawn)
    {
        std::vector<int> IDs;
        std::vector<int> increment;

        for (auto pawn_ptr : data.pawns)
        {
            // subtract 1 point of pawn's owner
            IDs.push_back(pawn_ptr->get_pawn()->getPlayerId());
            increment.push_back(-1);

            // add 1 point to current player if not destroy own pawn
            if (game_state->currentPlayer() != pawn_ptr->get_pawn()->getPlayerId())
            {
                IDs.push_back(game_state->currentPlayer());
                increment.push_back(1);
            }

            // remove pawn from hex
            game_board->removePawn(pawn_ptr->get_pawn()->getId());

            // remove pawn from graphic_pawn_list
            auto it = graphic_pawn_list.find(pawn_ptr->get_pawn()->getId());
            if (it != graphic_pawn_list.end())
            {
                graphic_pawn_list.erase(it);
            }

            // remove graphic pawn from scene
            scene->removeItem(pawn_ptr);

            // free memory of graphic pawn
            delete pawn_ptr;
        }
        // clear pawns from data
        data.pawns.clear();

        if (IDs.size() !=0)
        {
            emit(update_point(IDs, increment));
        }

        // check if existing players change
        update_existing_player();
    }

    // remove transport
    if (transport)
    {
        for (auto transport_ptr : data.transports)
        {
            // remove transport from hex
            game_board->removeTransport(transport_ptr->get_transport()->getId());

            // remove pawn from graphic_transport_list
            auto it = graphic_transport_list.find(transport_ptr->get_transport()->getId());
            if (it != graphic_transport_list.end())
            {
                graphic_transport_list.erase(it);
            }

            // remove graphic transport from scene
            scene->removeItem(transport_ptr);

            // free memory of graphic transport
            delete transport_ptr;
        }
        // clear transport from data
        data.transports.clear();
    }

    // remove actor
    if (actor)
    {
        for (auto actor_ptr : data.actors)
        {
            // remove pawn from hex
            game_board->removeActor(actor_ptr->get_actor()->getId());

            // remove pawn from graphic_actor_list
            auto it = graphic_actor_list.find(actor_ptr->get_actor()->getId());
            if (it != graphic_actor_list.end())
            {
                graphic_actor_list.erase(it);
            }

            // remove graphic pawn from scene
            scene->removeItem(actor_ptr);

            // free memory of graphic pawn
            delete actor_ptr;
        }
        // clear pawns from data
        data.actors.clear();
    }
}

void HexBoard::enable_pawn_movement()
{
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

void HexBoard::disable_pawn_movement()
{
    emit(set_pawn_movement(false, current_player_pawn_list));
    current_player_pawn_list.clear();
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




