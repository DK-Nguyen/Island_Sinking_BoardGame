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
    graphic_hex_list.clear();
    graphic_actor_list.clear();
    graphic_pawn_list.clear();
    graphic_transport_list.clear();
    pawn_vertices.clear();
    pawn_colors.clear();
    current_player_pawn_list.clear();
    movable_actors.clear();
    movable_transports.clear();
    data_map.clear();
    hex_centers.clear();
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
- verify logic versus implementation

***/
{
    std::cerr << "Tile is flipped \n";

    if (game_state->currentGamePhase() != 2)
    {
        return;
    }

    auto graphic_hex = graphic_hex_list[id];
    auto coord = graphic_hex->get_hex()->getCoordinates();
    try {
        auto flip_result = game_engine->flipTile(coord);
        std::cerr << "flip result: " << flip_result << "\n";

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
            std::cerr << "hex type after flipping: " << graphic_hex->get_hex()->isWaterTile() << "\n";

            // create new actor/transport
            if (flip_result.compare("boat")==0)
            {
                auto boat = std::make_shared<Common::Boat>(current_transport_id);
                game_board->addTransport(boat, graphic_hex->get_hex()->getCoordinates());
                add_transport(current_transport_id, boat);
                current_transport_id++;

                std::cerr << "finish creating graphic actor/tranposrt \n";
            }
            else if (flip_result.compare("dolphin")==0)
            {
                auto dolphin = std::make_shared<Common::Dolphin>(current_transport_id);
                game_board->addTransport(dolphin, graphic_hex->get_hex()->getCoordinates());
                add_transport(current_transport_id, dolphin);
                current_transport_id++;

                std::cerr << "finish creating graphic actor/tranposrt \n";
            }

            else if (flip_result.compare("kraken")==0)
            {
                auto kraken = std::make_shared<Common::Kraken>(current_actor_id);
                game_board->addActor(kraken, graphic_hex->get_hex()->getCoordinates());
                add_actor(current_actor_id, kraken);
                current_actor_id++;

                std::cerr << "finish creating graphic actor/tranposrt \n";
            }

            else if (flip_result.compare("vortex")==0)
            {
                std::cerr << "creating graphic vortex \n";

                // create vortex
                auto vortex = std::make_shared<Common::Vortex>(current_actor_id);
                game_board->addActor(vortex, graphic_hex->get_hex()->getCoordinates());
                add_actor(current_actor_id, vortex);

                // perform action
                do_vortex_action(current_actor_id);
                current_actor_id++;

                std::cerr << "finish creating graphic actor/tranposrt \n";

            }

            else if (flip_result.compare("shark")==0)
            {
                std::cerr << "creating graphic shark \n";

                auto shark = std::make_shared<Common::Shark>(current_actor_id);
                game_board->addActor(shark, graphic_hex->get_hex()->getCoordinates());
                add_actor(current_actor_id, shark);
                current_actor_id++;

                std::cerr << "finish creating graphic actor/tranposrt \n";
            }

            else if (flip_result.compare("seamunster")==0)
            {
                auto seamunster = std::make_shared<Common::Seamunster>(current_actor_id);
                std::cerr << "finish creating actor ptr \n";

                game_board->addActor(seamunster, graphic_hex->get_hex()->getCoordinates());
                std::cerr << "finish adding actor to gameboard \n";

                add_actor(current_actor_id, seamunster);
                std::cerr << "finish creating graphic actor \n";

                current_actor_id++;

                std::cerr << "finish creating graphic actor/tranposrt \n";
            }

            else
            {
                std::cerr << "the return actor/transport does not match! \n";
                std::cerr << flip_result << "\n";
                exit(0);
            }

            // change to stage 3
            change_stage(3);

        }
    }
    catch (Common::IllegalMoveException exception)
    {
        std::cerr << "******EXCEPTION: " << exception.msg() << "\n";
    }

    update_existing_player();
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

if no actor or transport exists yet, change to stage 1

TODO
----
- verify logic versus implementation

***/
{
    std::cerr << "wheel output: " << wheel_output.first << ", " << wheel_output.second << "\n";
    wheel_output_ = wheel_output;
    bool exist_character = false;

    // if dolphin, allow movement of pawn and dolphin
    if (wheel_output.first.compare("dolphin")==0)
    {
        // set graphic dolphin movable, also set current player's pawn on that hex movable
        for (auto transport : graphic_transport_list)
        {
            auto transport_ptr = transport->get_transport();

            if (transport_ptr->getTransportType().compare("dolphin")==0 && transport_ptr->canMove(game_state->currentPlayer()))
            {
                exist_character = true;
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
        for (auto actor : graphic_actor_list)
        {
            if (actor->get_actor()->getActorType().compare(wheel_output.first) == 0)
            {
                exist_character = true;
                movable_actors.push_back(actor->get_actor()->getId());
            }
        }

        if (movable_actors.size() > 0)
        {
            emit set_actor_movement(true, movable_actors);
        }
    }

    // if no character return by wheel exists in game board, change to next player's turn
    if (!exist_character)
    {
        change_stage(1);
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
            - if pawn is moved to transport in new hex
                - if transport in new hex is full
                    - put pawn to old position
                - if transport in new hex is not full
                    - add graphic pawn to graphic transport
                    - move pawn in backend
                    - update move left in backend, controlboard
                    - if no move left:
                        - if island is sunk:
                            - set to stage 3
                            - enable wheel click
                        - if island is not sunk:
                            - set to stage 2
                        - disable pawn movement
                        - clear current_player_pawn_list
                        - update stage in controlboard

            - if pawn is not moved to transport in new hex
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

            - if pawn was on transport in old hex and pawn is moved to new hex
                - remove graphic pawn from graphic transport
                - remove pawn from transport



 TODO:
 - check actual implementation versus logic description

 * ***/
{
    std::cerr << "pawn is moved\n";

    // get pawn 2D position
    auto old_cube_pos = plane_to_cube(old_pos.rx()/board_scale + 0.5, old_pos.ry()/board_scale+ 0.5);
    auto new_cube_pos = plane_to_cube(new_pos.rx()/board_scale + 0.5, new_pos.ry()/board_scale+ 0.5);

    auto old_data = data_map[cube_to_string(old_cube_pos)];
    auto new_data = data_map[cube_to_string(new_cube_pos)];

    // if not moving to any hex
    if (new_cube_pos == Common::CubeCoordinate(-1, -1, -1))
    {
        std::cerr << "pawn is moved outside of the gameboard, invalid movement \n";
        graphic_pawn_list[pawn_id]->setPos(old_pos);
    }

    print_cube(old_cube_pos);
    print_cube(new_cube_pos);

    // moving within the same hex
    if (old_cube_pos == new_cube_pos)
    {
        for (auto it : data_map[cube_to_string(old_cube_pos)].transports)
        {
            // if pawn is moved from outside to transport
            if (is_pawn_on_transport(new_pos, it) && !it->get_transport()->isPawnInTransport(graphic_pawn_list[pawn_id]->get_pawn()))
            {
                // full transport, move back to old pos
                if (it->is_full())
                {
                    graphic_pawn_list[pawn_id]->setPos(old_pos);
                }

                // if not full, add pawn to graphic transport and backend transport
                else
                {
                    std::cerr << "pawn is moved onto transport \n";
                    it->add_pawn(graphic_pawn_list[pawn_id]);
                    it->get_transport()->addPawn(graphic_pawn_list[pawn_id]->get_pawn());
                    it->allow_movement(true);
                }
            }

            // if pawn is moved out of transport
            if (is_pawn_on_transport(old_pos, it) && !is_pawn_on_transport(new_pos, it))
            {
                std::cerr << "pawn is moved out of transport \n";
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
        // in Stage 3, pawn can only move within hex: set to old position
        if (game_state->currentGamePhase()==3)
        {
            graphic_pawn_list[pawn_id]->setPos(old_pos);
        }

        // in stage 1, pawn can move both within and out of hex
        else
        {
            int move_left = game_engine->checkPawnMovement(old_cube_pos, new_cube_pos, pawn_id);
            std::cerr << "move left returned by checking movement: " << move_left << "\n";

            // if move is not valid: move back to old position
            if (move_left < 0)
            {
                graphic_pawn_list[pawn_id]->setPos(old_pos);
            }

            // if move is valid
            else
            {
                bool moved_to_transport = false;
                bool pawn_is_moved = false;

                // check if pawn is moved to transport on new hex
                for (auto it : new_data.transports)
                {

                    // if pawn is moved to transport
                    if (is_pawn_on_transport(new_pos, it))
                    {
                        std::cerr << "pawn is moved to transport on new hex: " <<  it->get_transport()->getTransportType() << "\n";

                        moved_to_transport = true;

                        // if transport is full, move back
                        if (it->is_full())
                        {
                            std::cerr << "transport is full, move back pawn to old position \n";
                            graphic_pawn_list[pawn_id]->setPos(old_pos);
                        }

                        // if transport is not full, perform movement
                        else
                        {
                            pawn_is_moved = true;

                            // add graphic pawn to  graphic transport
                            it->add_pawn(graphic_pawn_list[pawn_id]);

                            // add pawn to transport in backend
                            it->get_transport()->addPawn(graphic_pawn_list[pawn_id]->get_pawn());

                            // perform movement in backend
                            game_engine->movePawn(old_cube_pos, new_cube_pos, pawn_id);
                            //game_state->setActionsLeft(move_left);


                            // emit change info in ControlBoard
                            emit(update_movement_left());

                            // if no move left afterwards
                            if (move_left == 0)
                            {
                                change_stage(2);
                            }
                            else
                            {
                                it->allow_movement(true);
                            }
                        }
                    }
                }

                // if not moving to transport on new hex
                if (!moved_to_transport)
                {
                    std::cerr << "pawn isn't moved to transport on new hex\n";
                    pawn_is_moved = true;

                    // move on backend
                    game_engine->movePawn(old_cube_pos, new_cube_pos, pawn_id);
                    //game_state->setActionsLeft(move_left);

                    // emit change info in ControlBoard
                    emit(update_movement_left());

                    // if no move left afterwards
                    if (move_left == 0)
                    {
                        change_stage(2);
                    }
                }


                if (pawn_is_moved)
                {

                    // check if pawn was on transport and has been moved to new hex, remove pawn from old transport
                    for (auto it : old_data.transports)
                    {
                        // if pawn is moved out of transport
                        if (is_pawn_on_transport(old_pos, it))
                        {
                            std::cerr << "pawn was moved out of transport, remove pawn from old transport\n";
                            // remove graphic pawn from graphical transport
                            it->remove_pawn(graphic_pawn_list[pawn_id]);

                            // remove pawn from transport in backend
                            it->get_transport()->removePawn(graphic_pawn_list[pawn_id]->get_pawn());
                        }
                    }

                    // remove pawns on data map of old hex, add pawns to data map of new hex
                    moveGraphicPawn(pawn_id, old_cube_pos, new_cube_pos);
                }
            }
        }
    }

    update();
    graphic_pawn_list[pawn_id]->setZValue(2);
}

void HexBoard::actor_is_moved(int actor_id, QPointF old_pos, QPointF new_pos)
/***
LOGIC
-----

Description: actor is moved only during 3rd stage, movement validity is checked using game engine

if move is not valid:
    - put back graphic actor to old position

if move is valid:
    - move actor in backend
    - check type of actor and perform corresponding actions (e.g., do_shark_action(),...)
    - do action function should handles both graphic and backend
    - change game stage to 1
    - change to next player
    - update current_player_pawn_list
    - update info on controlboard
    - enable pawn movement

TODO
----
- write implementation
- verify logic versus implementation

***/
{
    // only accept at stage 3
    if (game_state->currentGamePhase()!=3)
    {
        return;
    }

    //
    auto actor = graphic_actor_list[actor_id];

    auto old_hex = actor->get_actor()->getHex()->getCoordinates();

    auto old_cube_pos = plane_to_cube(old_pos.rx()/board_scale+3, old_pos.ry()/board_scale + 3);
    auto new_cube_pos = plane_to_cube(new_pos.rx()/board_scale+3, new_pos.ry()/board_scale + 3);

    // if not moving to any hex
    if (new_cube_pos == Common::CubeCoordinate(-1,-1,-1))
    {
        std::cerr << "actor is moved out of gameboard, move back to old position \n";
        graphic_actor_list[actor_id]->setPos(old_pos);
    }

    if (!(old_cube_pos == new_cube_pos))
    {
        std::cerr << "actor moved out of hex \n";
        print_cube(old_hex);
        print_cube(old_cube_pos);
        print_cube(new_cube_pos);


        // if move is valid
        if (checkActorMovement(old_cube_pos, new_cube_pos, actor_id) && game_engine->checkActorMovement(old_cube_pos, new_cube_pos, actor_id, wheel_output_.second))
        {
            std::cerr << "actor movement is valid \n";

            // move in backend
            game_engine->moveActor(old_cube_pos, new_cube_pos, actor->get_actor()->getId(), wheel_output_.second);

            std::cerr << "successfully move actor in backend \n";

            // do action both backend and graphically
            if (actor->get_actor()->getActorType().compare("kraken")==0)
            {
                do_kraken_action(actor->get_actor()->getId());
            }
            else if (actor->get_actor()->getActorType().compare("shark")==0)
            {
                do_shark_action(actor->get_actor()->getId());
            }

            else if (actor->get_actor()->getActorType().compare("seamunster")==0)
            {
                do_seamunster_action(actor->get_actor()->getId());
            }

            std::cerr << "successfully do action graphic and backend \n";

            // disable actor movement
            emit set_actor_movement(false, movable_actors);
            movable_actors.clear();

            // change to stage 1
            change_stage(1);

            // remove actor on data_map of old hex, add actor to data_map of new hex
            moveGraphicActor(actor_id, old_cube_pos, new_cube_pos);

        }

        // if move is invalid, move back
        else
        {
            std::cerr << "actor movement is invalid \n";
            actor->setPos(old_pos);
        }

    }

    update_existing_player();
}

void HexBoard::transport_is_moved(int transport_id, QPointF old_pos, QPointF new_pos)
/***
LOGIC
-----

Description: perform transport movement and check if target position and surrounding hexes has actors that can
destroy the transport or pawns on transport.

NOTE: version 3.3.0 specifies transport carrying pawn with dive option


stage 1: both dolphin and boat can move
stage 3: only dolphin can move

- if move is not valid:
    - return transport to old position
- if move is valid:
    - should return #move left
    - move in backend usign game engine
    - move graphic pawns on transport with direction new_pos - old_pos
    - call update to move left on controlboard

- if boat:
    - if new hex has seamunster:
        - call do_seamunster_action()
    - if new hex has kraken:
        - call do_kraken_action()

- if stage 1:
    - if no move left:
        change to stage 2

- if stage 3:
    change to stage 3

TODO
----
- implement when new features from course side come
- verify implementation versus logic

***/

{
    // get corresponding cube position
    auto old_cube_pos = plane_to_cube(old_pos.rx()/board_scale + 3.0, old_pos.ry()/board_scale + 5.0);
    auto new_cube_pos = plane_to_cube(new_pos.rx()/board_scale + 3.0, new_pos.ry()/board_scale + 5.0);
    auto graphic_transport = graphic_transport_list[transport_id];

    std::cerr << "transport is moved \n";
    print_cube(graphic_transport_list[transport_id]->get_transport()->getHex()->getCoordinates());
    print_cube(old_cube_pos);
    print_cube(new_cube_pos);

    // if not moving to any hex
    if (new_cube_pos == Common::CubeCoordinate(-1,-1,-1))
    {
        graphic_transport->setPos(old_pos);
    }


    bool can_move = true;

    // if move to new hex
    if (old_cube_pos == new_cube_pos)
    {
        // move graphic pawns
        for (auto pawn : graphic_transport->get_pawn_list())
        {
            std::cerr << "moving pawns on transport on the same hex \n";
            pawn->setPos(pawn->scenePos() + new_pos - old_pos);
        }
    }
    else
    {

        // if satisfy additional constraint
        if (checkTransportMovement(old_cube_pos, new_cube_pos, transport_id))
        {
            std::string moves;
            if (game_state->currentGamePhase() == 1)
            {
                moves = std::to_string(game_state->getActionsLeft());
            }
            else
            {
                moves = wheel_output_.second;
            }
            auto move_left = game_engine->checkTransportMovement(old_cube_pos, new_cube_pos, transport_id, moves);

            // if invalid movement
            if (move_left < 0)
            {
                can_move = false;
            }

            // if move is valid
            else
            {
                // move backend in stage 1
                if (game_state->currentGamePhase()==1)
                {
                    std::cerr << "moving transport in stage 1\n";
                    game_engine->moveTransport(old_cube_pos, new_cube_pos, transport_id);
                    emit update_movement_left();
                }

                // move backend in stage 3
                else
                {
                    std::cerr << "moving transport in stage 3 \n";
                    game_engine->moveTransportWithSpinner(old_cube_pos, new_cube_pos, transport_id, wheel_output_.second);
                }

                moveGraphicTransport(transport_id, old_cube_pos, new_cube_pos);

                // move graphic pawns
                for (auto pawn : graphic_transport->get_pawn_list())
                {
                    std::cerr << "moving pawns on transport \n";
                    pawn->setPos(pawn->scenePos() + new_pos - old_pos);
                    moveGraphicPawn(pawn->get_pawn()->getId(), old_cube_pos, new_cube_pos);
                }

                // perform action of actors in new hex
                if (graphic_transport->get_transport()->getTransportType().compare("boat")==0)
                {
                    auto data = data_map[cube_to_string(new_cube_pos)];
                    for (auto actor : data.actors)
                    {
                        if (actor->get_actor()->getActorType().compare("seamunster")==0)
                        {
                            do_seamunster_action(actor->get_actor()->getId());
                        }

                        else if (actor->get_actor()->getActorType().compare("kraken")==0)
                        {
                            do_kraken_action(actor->get_actor()->getId());
                        }
                    }

                }

                if (graphic_transport->get_transport()->getTransportType().compare("dolphin")==0)
                {
                    auto data = data_map[cube_to_string(new_cube_pos)];
                    for (auto actor : data.actors)
                    {
                        if (actor->get_actor()->getActorType().compare("shark")==0)
                        {
                            do_shark_action(actor->get_actor()->getId());
                        }
                    }

                }

                // if stage 1 and no move left
                std::cerr << "current game phase: " << game_state->currentGamePhase() << "\n";
                std::cerr << "move left: " << move_left << "\n";
                if (game_state->currentGamePhase()==1 && move_left==0)
                {
                    std::cerr << "move left: " << move_left << "\n";
                    std::cerr << "changing to stage 2 \n";
                    change_stage(2);
                }

                if (game_state->currentGamePhase()==3)
                {
                    change_stage(1);
                }
            }
        }
        else
        {
            can_move = false;
        }

    }

    // if cannot move, set to old position
    if (!can_move)
    {
        graphic_transport_list[transport_id]->setPos(old_pos);
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
    graphic_actor->show();
    graphic_actor->setScale(board_scale*ACTOR_SCALE);

    std::cerr << "hex center: " << actor_pos.rx() << ", " << actor_pos.ry() << "\n";

    actor_pos.setX(actor_pos.rx() + board_scale);
    actor_pos.setY(actor_pos.ry() + board_scale);

    graphic_actor->setPos(actor_pos);

    actor_pos = graphic_actor->scenePos();
    std::cerr << "actor position: " << actor_pos.rx() << ", " << actor_pos.ry() << "\n";

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
    update();
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
    // add hex centers to list
    hex_centers.push_back(std::make_pair(cube_to_hex_center(hex_ptr->getCoordinates()), hex_ptr->getCoordinates()));

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
    auto pos = std::make_pair(x,y);
    double min_dis = std::numeric_limits<double>::max();

    for (auto center : hex_centers)
    {
        auto dist = euclidean_distance(pos, center.first);
        if (dist < min_dis)
        {
            min_dis = dist;
            output_pos = center.second;
        }
    }

    if (min_dis > 9)
    {
        output_pos = Common::CubeCoordinate(-1, -1, -1);
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
    game_state->setActionsLeft(3);
    emit update_movement_left();
}

void HexBoard::update_existing_player()
// update list of survived players
{
    std::cerr << "updating players is called \n";
    std::vector<unsigned int> removed_players;

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
            std::cerr << "player " << player->getPlayerId() << " has no pawn left \n";
            removed_players.push_back(i);
        }
    }

    // remove player from vector of players
    for (auto i=0; i < removed_players.size(); i++)
    {
        std::cerr << "removing player " << players[i]->getPlayerId() << "\n";
        players.erase(players.begin() + removed_players[i]-i);
    }

    std::cerr << "number of players left: " << players.size() << "\n";
    // if only 1 player left, clear data and emit game over
    if (players.size()==1)
    {
        clear();
        emit game_over(game_state->get_player_name(players[0]->getPlayerId()));
    }
    else if (players.size()==0)
    {
        clear();
        emit game_over("");
    }
}

void HexBoard::do_vortex_action(int id)
// destroy everything on current hex and neighboring hexes
{
    std::cerr << "do vortex action \n";

    HexData data;
    auto graphic_vortex = graphic_actor_list[id];
    auto hex = graphic_vortex->get_actor()->getHex();
    auto neighbors = hex->getNeighbourVector();

    // destroy everything on current hex

    auto key = cube_to_string(hex->getCoordinates());
    data_map[key] = remove_data(key, true, "boat", "shark");
    data_map[key] = remove_data(key, false, "dolphin", "kraken");
    data_map[key] = remove_data(key, false, "", "seamunster");
    data_map[key] = remove_data(key, false, "", "vortex");

    std::cerr << "successfully remove data on current hex \n";

    // destroy everything on neighboring water hexes
    for (auto neighbor_hex : neighbors)
    {
        // check if water hex, destroy thing
        if (game_board->getHex(neighbor_hex)->isWaterTile())
        {
            auto key = cube_to_string(neighbor_hex);
            data_map[key] = remove_data(key, true, "boat", "shark");
            data_map[key] = remove_data(key, false, "dolphin", "kraken");
            data_map[key] = remove_data(key, false, "", "seamunster");
        }
    }

    std::cerr << "successfully remove data on neighboring hexes \n";
}

void HexBoard::do_kraken_action(int id)
/***
description: destroy boat on current hex, pawns are put on the sea again

LOGIC
-----
- call doAction() on backend
- if current hex has boat
    - destroy graphical boat

TODO
----
- verify implementation versus logic

***/
{
    std::cerr << "do kraken action \n";

    auto kraken = graphic_actor_list[id];
    auto coord = kraken->get_actor()->getHex()->getCoordinates();

    // destroy graphical boats
    data_map[cube_to_string(coord)] = remove_data(cube_to_string(coord), false, "boat", "");

    std::cerr << "successfully remove boat \n";

}

void HexBoard::do_shark_action(int id)
/***
description: shark eats pawns on the hex

LOGIC
-----

- remove graphical and backend pawns and dolphin


TODO
----
- verify implementation versus logic
 ***/
{
    std::cerr << "do shark actions \n";

    auto shark = graphic_actor_list[id];
    auto coord = shark->get_actor()->getHex()->getCoordinates();

    // destroy graphical and backend pawns
    data_map[cube_to_string(coord)] = remove_data(cube_to_string(coord), true, "dolphin", "");
    std::cerr << "successfully remove pawns \n";
}

void HexBoard::do_seamunster_action(int id)
/***
description: shark eats pawns and boat on the hex

LOGIC
-----

- remove graphical pawn and boat
- call doAction()


TODO
----
- verify implementation versus logic
 ***/
{
    std::cerr << "do seamunster action \n";
    auto seamunster = graphic_actor_list[id];
    auto coord = seamunster->get_actor()->getHex()->getCoordinates();

    // destroy graphical and backend boats and pawns
    data_map[cube_to_string(coord)] = remove_data(cube_to_string(coord), true, "boat", "");
    std::cerr << "successfully remove boat and pawns \n";

}

std::string HexBoard::cube_to_string(Common::CubeCoordinate coord)
// convert cube coordinate to string as key to hash table
{
    return std::to_string(coord.x) + "+" + std::to_string(coord.y) + "+" + std::to_string(coord.z);
}

HexData HexBoard::remove_data(std::string key, bool pawn, std::string transport_type, std::string actor_type)
// perform both graphic and backend data remove with given flags
{
    auto data = data_map[key];
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

            // remove pawn from transport on the same hex if pawn is on transport
            std::cerr << "number of transport on tile: " << data.transports.size() << "\n";
            for (auto transport : data.transports)
            {
                if (transport->get_transport()->isPawnInTransport(pawn_ptr->get_pawn()))
                {
                    std::cerr << "removing pawn from transport \n";
                    transport->get_transport()->removePawn(pawn_ptr->get_pawn());
                    transport->remove_pawn(pawn_ptr);
                }
            }

            // remove pawn from hex
            std::cerr << "removing pawn on game board \n";
            game_board->removePawn(pawn_ptr->get_pawn()->getId());

            // remove pawn from graphic_pawn_list
            auto it = graphic_pawn_list.find(pawn_ptr->get_pawn()->getId());
            if (it != graphic_pawn_list.end())
            {
                std::cerr << "removing graphic pawn \n";
                graphic_pawn_list.erase(it);
            }


            // remove graphic pawn from scene
            scene->removeItem(pawn_ptr);

            delete pawn_ptr;
        }
        // clear pawns from data
        data.pawns.clear();

        if (IDs.size() !=0)
        {
            emit(update_point(IDs, increment));
        }
    }

    // remove transport
    if (transport_type.compare("")!=0)
    {
        std::cerr << "start removing transport \n";

        std::vector<unsigned int> indices;
        unsigned int increment = 0;
        std::cerr << "number of transport on tile: " << data.transports.size() << "\n";
        for (auto transport_ptr : data.transports)
        {
            // if matching the type
            if (transport_ptr->get_transport()->getTransportType().compare(transport_type)==0)
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

                indices.push_back(increment);
            }

            increment++;
        }

        for (unsigned int i=0; i < indices.size(); i++)
        {
            auto it = data.transports.begin();
            advance(it, indices[i]-i);
            data.transports.erase(it);
        }
    }

    // remove actor
    if (actor_type.compare("")!=0)
    {
        std::vector<unsigned int> indices;
        unsigned int increment = 0;

        for (auto actor_ptr : data.actors)
        {
            // if matching the type
            if (actor_ptr->get_actor()->getActorType().compare(actor_type)==0)
            {
                // remove transport from hex
                game_board->removeActor(actor_ptr->get_actor()->getId());

                // remove pawn from graphic_transport_list
                auto it = graphic_actor_list.find(actor_ptr->get_actor()->getId());
                if (it != graphic_actor_list.end())
                {
                    graphic_actor_list.erase(it);
                }

                // remove graphic transport from scene
                scene->removeItem(actor_ptr);

                // free memory of graphic transport
                delete actor_ptr;

                indices.push_back(increment);
            }

            increment++;
        }

        for (unsigned int i=0; i < indices.size(); i++)
        {
            auto it = data.actors.begin();
            advance(it, indices[i]-i);
            data.actors.erase(it);
        }
    }

    return data;
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

void HexBoard::enable_transport_movement()
{
    std::cerr << "enabling transport movement \n";
    for (auto transport_it : graphic_transport_list)
    {
        for (auto pawn_id : current_player_pawn_list)
        {
            std::cerr << "current player pawn id: " << pawn_id << "\n";
            std::cerr << "transport id: " << transport_it->get_transport()->getId() << "\n";
            if (transport_it->get_transport()->isPawnInTransport(graphic_pawn_list[pawn_id]->get_pawn()))
            {
                movable_transports.push_back(transport_it->get_transport()->getId());
                transport_it->allow_movement(true);
            }
        }
    }
}



void HexBoard::disable_pawn_movement()
{
    emit(set_pawn_movement(false, current_player_pawn_list));
    current_player_pawn_list.clear();
}

void HexBoard::disable_transport_movement()
{
    for (auto id : movable_transports)
    {
        graphic_transport_list[id]->allow_movement(false);
    }
    movable_transports.clear();
}

void HexBoard::change_stage(int stage)
{
    if (stage == 1)
    {
        // clear current player's pawns
        current_player_pawn_list.clear();

        // change to next player
        change_player_turn(game_engine->currentPlayer());
        emit update_player_turn();

        // enable pawn movement
        enable_pawn_movement();

        // change stage
        game_state->changeGamePhase(Common::GamePhase(1));
        emit update_stage();

        // enable transport movement
        enable_transport_movement();

        // disable wheel output
    }

    if (stage == 2)
    {
        disable_transport_movement();
        // if island is not sunk
        if (!game_board->isIslandSunk())
        {
            game_state->changeGamePhase(Common::GamePhase(2));
            emit update_stage();
            disable_pawn_movement();

        }

        // if island is sunk
        else
        {
            game_state->changeGamePhase(Common::GamePhase(3));
            emit update_stage();

            disable_pawn_movement();
            emit set_wheel_click(true);
        }
    }

    if (stage == 3)
    {
        game_state->changeGamePhase(Common::GamePhase(3));
        emit update_stage();
        emit set_wheel_click(true);
    }
}

void HexBoard::print_cube(Common::CubeCoordinate coord)
{
    std::cerr << "cube: " << coord.x << ", " << coord.y << ", " << coord.z << "\n";
}

bool HexBoard::checkActorMovement(Common::CubeCoordinate old_pos, Common::CubeCoordinate new_pos, int id)
/***
Description: check actor movement validity, add additional constraint on actor movement

LOGIC
-----

- if shark:
    shark cannot go to hex with : boat, seamunster
- if kraken:
    kraken cannot go to hex with: seamunster
- if seamunster:
    cannot go to hex with: shark
***/
{
    bool allowed = true;
    auto graphic_actor = graphic_actor_list[id];

    if (graphic_actor->get_actor()->getActorType().compare("shark")==0)
    {
        auto target_data = data_map[cube_to_string(new_pos)];
        for (auto actor : target_data.actors)
        {
            if (actor->get_actor()->getActorType().compare("seamunster")==0)
            {
                allowed = false;
                return allowed;
            }
        }

        for (auto transport : target_data.transports)
        {
            if (transport->get_transport()->getTransportType().compare("boat")==0)
            {
                allowed = false;
                return allowed;
            }
        }
    }

    if (graphic_actor->get_actor()->getActorType().compare("kraken")==0)
    {
        auto target_data = data_map[cube_to_string(new_pos)];
        for (auto actor : target_data.actors)
        {
            if (actor->get_actor()->getActorType().compare("seamunster")==0)
            {
                allowed = false;
                return allowed;
            }
        }
    }

    if (graphic_actor->get_actor()->getActorType().compare("seamunster")==0)
    {
        auto target_data = data_map[cube_to_string(new_pos)];
        for (auto actor : target_data.actors)
        {
            if (actor->get_actor()->getActorType().compare("shark")==0)
            {
                allowed = false;
                return allowed;
            }
        }
    }

    return allowed;
}

bool HexBoard::checkTransportMovement(Common::CubeCoordinate old_pos, Common::CubeCoordinate new_pos, int id)
/***
Description: check transport movement, add additional constraint to check movement

LOGIC
-----
if exist transport in target hex, movement is not allowed
 ***/
{
    bool allowed = true;
    auto target_data = data_map[cube_to_string(new_pos)];
    if (target_data.transports.size() > 0)
    {
        allowed = false;
    }

    return allowed;
}

void HexBoard::moveGraphicPawn(int pawn_id, Common::CubeCoordinate old_pos, Common::CubeCoordinate new_pos)
{
    auto old_data = data_map[cube_to_string(old_pos)];
    auto new_data = data_map[cube_to_string(new_pos)];

    int index = 0;
    for (auto it : old_data.pawns)
    {
        if (it->get_pawn() == graphic_pawn_list[pawn_id]->get_pawn())
        {
            break;
        }
        index++;
    }

    auto it = old_data.pawns.begin();
    advance(it, index);
    old_data.pawns.erase(it);
    data_map[cube_to_string(old_pos)] = old_data;

    data_map[cube_to_string(new_pos)].pawns.push_back(graphic_pawn_list[pawn_id]);
}

void HexBoard::moveGraphicTransport(int transport_id, Common::CubeCoordinate old_pos, Common::CubeCoordinate new_pos)
{
    int index = 0;
    for (auto it : data_map[cube_to_string(old_pos)].transports)
    {
        if (it->get_transport()->getId() == graphic_transport_list[transport_id]->get_transport()->getId())
        {
            break;
        }
        index++;
    }

    auto it = data_map[cube_to_string(old_pos)].transports.begin();
    advance(it, index);
    data_map[cube_to_string(old_pos)].transports.erase(it);
    data_map[cube_to_string(new_pos)].transports.push_back(graphic_transport_list[transport_id]);
}

void HexBoard::moveGraphicActor(int actor_id, Common::CubeCoordinate old_pos, Common::CubeCoordinate new_pos)
{
    auto old_data = data_map[cube_to_string(old_pos)];
    auto new_data = data_map[cube_to_string(new_pos)];

    int index = 0;
    for (auto it : old_data.actors)
    {
        if (it->get_actor()->getId() == graphic_actor_list[actor_id]->get_actor()->getId())
        {
            break;
        }
        index++;
    }

    auto it = old_data.actors.begin();
    advance(it, index);
    old_data.actors.erase(it);
    data_map[cube_to_string(old_pos)] = old_data;

    new_data.actors.push_back(graphic_actor_list[actor_id]);

    data_map[cube_to_string(new_pos)] = new_data;

    return;
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




