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

namespace UI{

HexBoard::HexBoard(std::shared_ptr<Common::IGameRunner> gameEnginePtr,
                       std::shared_ptr<Student::GameBoard> gameboardPtr,
                       std::shared_ptr<Student::GameState> gamestatePtr,
                       std::vector<std::shared_ptr<Common::IPlayer>> gamePlayers,
                       int width,
                       int height,
                       int boardScale,
                       QWidget* parent)
    : QGraphicsView (parent)

{

    this->gameEngine_ = gameEnginePtr;
    this->gameBoard_ = gameboardPtr;
    this->gameState_ = gamestatePtr;
    this->players = gamePlayers;
    this->boardScale_ = boardScale;
    this->width = width;
    this->height = height;
    this->currentActorId_ = 100;
    this->currentTransportId_ = 500;

    // 8 distinct colors for maximum 8 players
    QVector<QColor> playerColors;
    playerColors.push_back(Qt::red);
    playerColors.push_back(Qt::blue);
    playerColors.push_back(Qt::white);
    playerColors.push_back(QColor(128, 0, 128));
    playerColors.push_back(QColor(165, 42, 42));
    playerColors.push_back(QColor(245, 130, 48));
    playerColors.push_back(QColor(255, 250, 200));
    playerColors.push_back(QColor(170, 255, 195));

    int playerCount = 0;
    for (auto player : gamePlayers)
    {
        auto name = QString::fromUtf8(gameState_->getPlayerName(player->getPlayerId()).c_str());
        pawnColors_[name] = playerColors[playerCount];
        playerCount += 1;
    }

    // set up the screen
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(width, height);

    hScrollbar_ = new QScrollBar(this);
    hScrollbar_->setMinimumHeight(100);

    vScrollbar_ = new QScrollBar(this);
    vScrollbar_->setMinimumWidth(100);

    setHorizontalScrollBar(hScrollbar_);
    setVerticalScrollBar(vScrollbar_);

    // set up the scene
    scene_ = new QGraphicsScene();
    scene_->setSceneRect(0,0,160*boardScale,180*boardScale);
    setScene(scene_);

    scene_->clear();
    populate();
}

void HexBoard::clear()
// clear all data on HexBoard
{
    graphicHexList.clear();
    graphicActorList_.clear();
    graphicPawnList_.clear();
    graphicTransportList_.clear();
    pawnVertices_.clear();
    pawnColors_.clear();
    currentPlayerPawnList_.clear();
    movableActors_.clear();
    movableTransports_.clear();
    dataMap_.clear();
    hexCenters.clear();
}


void HexBoard::hexClicked(int id)
/***
LOGIC
-----
Description: This slot is called when graphic hex is clicked

if not during stage 2: ignore
if stage 2:
- if can flip:
    - change color of graphic hex
    - create new graphic actor/transport and add to backend, graphic
    - if vortex: do vortex action
    - change stage to 3
    - update controlboard
    - enable wheel click
    - update existing players (in case vortex performs action)

***/
{
    if (gameState_->currentGamePhase() != 2)
    {
        return;
    }

    auto graphicHex = graphicHexList[id];
    auto coord = graphicHex->getHex()->getCoordinates();
    try {
        auto flipResult = gameEngine_->flipTile(coord);

        // fail the flip
        if (flipResult.compare("") != 0)
        {
            // change color of graphic hex
            graphicHexList[id]->flip();
            // create new actor/transport
            if (flipResult.compare("boat")==0)
            {
                auto boat = std::make_shared<Common::Boat>(currentTransportId_);
                gameBoard_->addTransport(boat, graphicHex->getHex()->getCoordinates());
                addTransport(currentTransportId_, boat);
                currentTransportId_++;
            }
            else if (flipResult.compare("dolphin")==0)
            {
                auto dolphin = std::make_shared<Common::Dolphin>(currentTransportId_);
                gameBoard_->addTransport(dolphin, graphicHex->getHex()->getCoordinates());
                addTransport(currentTransportId_, dolphin);
                currentTransportId_++;
            }

            else if (flipResult.compare("kraken")==0)
            {
                auto kraken = std::make_shared<Common::Kraken>(currentActorId_);
                gameBoard_->addActor(kraken, graphicHex->getHex()->getCoordinates());
                addActor(currentActorId_, kraken);
                currentActorId_++;
            }

            else if (flipResult.compare("vortex")==0)
            {

                // create vortex
                auto vortex = std::make_shared<Common::Vortex>(currentActorId_);
                gameBoard_->addActor(vortex, graphicHex->getHex()->getCoordinates());
                addActor(currentActorId_, vortex);

                // perform action
                doVortexAction(currentActorId_);
                currentActorId_++;

            }

            else if (flipResult.compare("shark")==0)
            {
                auto shark = std::make_shared<Common::Shark>(currentActorId_);
                gameBoard_->addActor(shark, graphicHex->getHex()->getCoordinates());
                addActor(currentActorId_, shark);
                currentActorId_++;
            }

            else if (flipResult.compare("seamunster")==0)
            {
                auto seamunster = std::make_shared<Common::Seamunster>(currentActorId_);

                gameBoard_->addActor(seamunster, graphicHex->getHex()->getCoordinates());

                addActor(currentActorId_, seamunster);

                currentActorId_++;
            }

            else
            {
                exit(0);
            }

            // change to stage 3
            changeStage(3);

        }
    }
    catch (Common::IllegalMoveException exception)
    {
        std::cerr << "******EXCEPTION: " << exception.msg() << "\n";
    }

    updateExistingPlayer();
}

void HexBoard::wheelClicked(std::pair<std::string, std::string> wheelOutput)
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

***/
{
    wheelOutput_ = wheelOutput;
    bool existCharacter = false;

    // if dolphin, allow movement of pawn and dolphin
    if (wheelOutput.first.compare("dolphin")==0)
    {
        // set graphic dolphin movable, also set current player's pawn on that hex movable
        for (auto transport : graphicTransportList_)
        {
            auto transportPtr = transport->getTransport();

            if (transportPtr->getTransportType().compare("dolphin")==0 && transportPtr->canMove(gameState_->currentPlayer()))
            {
                existCharacter = true;
                // set dolphin movable
                transport->allowMovement(true);
                auto coord = transport->getTransport()->getHex()->getCoordinates();
                auto key = cubeToString(coord);

                currentPlayerPawnList_.clear();
                for (auto pawn : dataMap_.at(key).pawns)
                {
                    if (pawn->getPawn()->getPlayerId() == gameState_->currentPlayer())
                    {
                        currentPlayerPawnList_.push_back(pawn->getPawn()->getId());
                    }
                }
            }
        }

        // set pawn movable
        if (currentPlayerPawnList_.size() > 0)
        {
            emit setPawnMovement(true, currentPlayerPawnList_);
        }
    }

    // if other actors, set movable
    else
    {
        for (auto actor : graphicActorList_)
        {
            if (actor->getActor()->getActorType().compare(wheelOutput.first) == 0)
            {
                existCharacter = true;
                movableActors_.push_back(actor->getActor()->getId());
            }
        }

        if (movableActors_.size() > 0)
        {
            emit setActorMovement(true, movableActors_);
        }
    }

    // if no character return by wheel exists in game board, change to next player's turn
    if (!existCharacter)
    {
        changeStage(1);
    }
}

void HexBoard::pawnIsMoved(int pawnId, QPointF oldPos, QPointF newPos)
/***
LOGIC
-----
Description: slot handles when graphic pawn is moved

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
        - move back pawn to old position (stage 3 only allows moving pawn to dolphin on same hex)
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
                    - update controlBoard
                    - if no move left:
                        - if island is sunk:
                            - set to stage 3
                        - if island is not sunk:
                            - set to stage 2

            - if pawn is not moved to transport in new hex
                - move pawn in backend
                - update controlBoard
                - if no move left:
                    - if island is sunk:
                        - set to stage 3
                    - if island is not sunk:
                        - set to stage 2

            - if pawn was on transport in old hex and pawn is moved to new hex
                - remove graphic pawn from graphic transport
                - remove pawn from transport

            - handle moving graphic pawns on dataMap
 * ***/
{
    // get pawn 2D position
    auto oldCubePos = planeToCube(oldPos.rx()/boardScale_ + 0.5, oldPos.ry()/boardScale_+ 0.5);
    auto newCubePos = planeToCube(newPos.rx()/boardScale_ + 0.5, newPos.ry()/boardScale_+ 0.5);

    auto oldData = dataMap_[cubeToString(oldCubePos)];
    auto newData = dataMap_[cubeToString(newCubePos)];

    // if not moving to any hex
    if (newCubePos == Common::CubeCoordinate(-1, -1, -1))
    {
        graphicPawnList_[pawnId]->setPos(oldPos);
    }


    // moving within the same hex
    if (oldCubePos == newCubePos)
    {
        for (auto it : dataMap_[cubeToString(oldCubePos)].transports)
        {
            // if pawn is moved from outside to transport
            if (isPawnOnTransport(newPos, it) && !it->getTransport()->isPawnInTransport(graphicPawnList_[pawnId]->getPawn()))
            {
                // full transport, move back to old pos
                if (it->isFull())
                {
                    graphicPawnList_[pawnId]->setPos(oldPos);
                }

                // if not full, add pawn to graphic transport and backend transport
                else
                {
                    it->addPawn(graphicPawnList_[pawnId]);
                    it->getTransport()->addPawn(graphicPawnList_[pawnId]->getPawn());
                    it->allowMovement(true);
                }
            }

            // if pawn is moved out of transport
            if (isPawnOnTransport(oldPos, it) && !isPawnOnTransport(newPos, it))
            {
                // put to new position
                graphicPawnList_[pawnId]->setPos(newPos);

                // remove from graphical transport
                it->removePawn(graphicPawnList_[pawnId]);

                // remove from backend
                it->getTransport()->removePawn(graphicPawnList_[pawnId]->getPawn());

            }
        }
    }

    // if pawn is moved to another hex
    else
    {
        // in Stage 3, pawn can only move within hex: set to old position
        if (gameState_->currentGamePhase()==3)
        {
            graphicPawnList_[pawnId]->setPos(oldPos);
        }

        // in stage 1, pawn can move both within and out of hex
        else
        {
            int moveLeft = gameEngine_->checkPawnMovement(oldCubePos, newCubePos, pawnId);

            // if move is not valid: move back to old position
            if (moveLeft < 0)
            {
                graphicPawnList_[pawnId]->setPos(oldPos);
            }

            // if move is valid
            else
            {
                bool movedToTransport = false;
                bool pawnIsMoved = false;

                // check if pawn is moved to transport on new hex
                for (auto it : newData.transports)
                {

                    // if pawn is moved to transport
                    if (isPawnOnTransport(newPos, it))
                    {

                        movedToTransport = true;

                        // if transport is full, move back
                        if (it->isFull())
                        {
                            graphicPawnList_[pawnId]->setPos(oldPos);
                        }

                        // if transport is not full, perform movement
                        else
                        {
                            pawnIsMoved = true;

                            // add graphic pawn to  graphic transport
                            it->addPawn(graphicPawnList_[pawnId]);

                            // add pawn to transport in backend
                            it->getTransport()->addPawn(graphicPawnList_[pawnId]->getPawn());

                            // perform movement in backend
                            gameEngine_->movePawn(oldCubePos, newCubePos, pawnId);
                            //game_state->setActionsLeft(move_left);


                            // emit change info in ControlBoard
                            emit(updateMovementLeft());

                            // if no move left afterwards
                            if (moveLeft == 0)
                            {
                                changeStage(2);
                            }
                            else
                            {
                                it->allowMovement(true);
                            }
                        }
                    }
                }

                // if not moving to transport on new hex
                if (!movedToTransport)
                {
                    pawnIsMoved = true;

                    // move on backend
                    gameEngine_->movePawn(oldCubePos, newCubePos, pawnId);
                    //game_state->setActionsLeft(move_left);

                    // emit change info in ControlBoard
                    emit(updateMovementLeft());

                    // if no move left afterwards
                    if (moveLeft == 0)
                    {
                        changeStage(2);
                    }
                }


                if (pawnIsMoved)
                {

                    // check if pawn was on transport and has been moved to new hex, remove pawn from old transport
                    for (auto it : oldData.transports)
                    {
                        // if pawn is moved out of transport
                        if (isPawnOnTransport(oldPos, it))
                        {
                            // remove graphic pawn from graphical transport
                            it->removePawn(graphicPawnList_[pawnId]);

                            // remove pawn from transport in backend
                            it->getTransport()->removePawn(graphicPawnList_[pawnId]->getPawn());
                        }
                    }

                    // remove pawns on data map of old hex, add pawns to data map of new hex
                    moveGraphicPawn(pawnId, oldCubePos, newCubePos);
                }
            }
        }
    }

    update();
    graphicPawnList_[pawnId]->setZValue(2);
}

void HexBoard::actorIsMoved(int actorId, QPointF oldPos, QPointF newPos)
/***
LOGIC
-----

Description: slot that handles movement of graphic actor

if move is not valid:
    - put back graphic actor to old position

if move is valid:
    - move actor in backend
    - check type of actor and perform corresponding actions (e.g., doSharkAction(),...)
    - change game stage to 1

***/
{
    // only accept at stage 3
    if (gameState_->currentGamePhase()!=3)
    {
        return;
    }

    //
    auto actor = graphicActorList_[actorId];

    auto oldHex = actor->getActor()->getHex()->getCoordinates();

    auto oldCubePos = planeToCube(oldPos.rx()/boardScale_+3, oldPos.ry()/boardScale_ + 3);
    auto newCubePos = planeToCube(newPos.rx()/boardScale_+3, newPos.ry()/boardScale_ + 3);

    // if not moving to any hex
    if (newCubePos == Common::CubeCoordinate(-1,-1,-1))
    {
        graphicActorList_[actorId]->setPos(oldPos);
    }

    if (!(oldCubePos == newCubePos))
    {

        // if move is valid
        if (checkActorMovement(newCubePos, actorId) && gameEngine_->checkActorMovement(oldCubePos, newCubePos, actorId, wheelOutput_.second))
        {

            // move in backend
            gameEngine_->moveActor(oldCubePos, newCubePos, actor->getActor()->getId(), wheelOutput_.second);

            // do action both backend and graphically
            if (actor->getActor()->getActorType().compare("kraken")==0)
            {
                doKrakenAction(actor->getActor()->getId());
            }
            else if (actor->getActor()->getActorType().compare("shark")==0)
            {
                doSharkAction(actor->getActor()->getId());
            }

            else if (actor->getActor()->getActorType().compare("seamunster")==0)
            {
                doSeamunsterAction(actor->getActor()->getId());
            }

            // disable actor movement
            emit setActorMovement(false, movableActors_);
            movableActors_.clear();

            // change to stage 1
            changeStage(1);

            // remove actor on data_map of old hex, add actor to data_map of new hex
            moveGraphicActor(actorId, oldCubePos, newCubePos);

        }

        // if move is invalid, move back
        else
        {
            actor->setPos(oldPos);
        }

    }

    updateExistingPlayer();
}

void HexBoard::transportIsMoved(int transportId, QPointF oldPos, QPointF newPos)
/***
LOGIC
-----

Description: perform transport movement and check if target position and surrounding hexes has actors that can
destroy the transport or pawns on transport.

stage 1: both dolphin and boat can move
stage 3: only dolphin can move

- if move is not valid:
    - return transport to old position
- if move is valid:
    - move in backend usign game engine
    - move graphic pawns on transport with direction new_pos - old_pos
    - update controlBoard

- if boat:
    - if new hex has seamunster:
        - call doSeamunsterAction()
    - if new hex has kraken:
        - call doKrakenAction()

- if stage 1:
    - if no move left:
        change to stage 2

- if stage 3:
    change to stage 1

***/

{
    // get corresponding cube position
    auto oldCubePos = planeToCube(oldPos.rx()/boardScale_ + 3.0, oldPos.ry()/boardScale_ + 5.0);
    auto newCubePos = planeToCube(newPos.rx()/boardScale_ + 3.0, newPos.ry()/boardScale_ + 5.0);
    auto graphicTransport = graphicTransportList_[transportId];

    // if not moving to any hex
    if (newCubePos == Common::CubeCoordinate(-1,-1,-1))
    {
        graphicTransport->setPos(oldPos);
    }


    bool canMove = true;

    // if move to new hex
    if (oldCubePos == newCubePos)
    {
        // move graphic pawns
        for (auto pawn : graphicTransport->getPawnList())
        {
            pawn->setPos(pawn->scenePos() + newPos - oldPos);
        }
    }
    else
    {

        // if satisfy additional constraint
        if (checkTransportMovement(oldCubePos, newCubePos, transportId))
        {
            std::string moves;
            if (gameState_->currentGamePhase() == 1)
            {
                moves = std::to_string(gameState_->getActionsLeft());
            }
            else
            {
                moves = wheelOutput_.second;
            }
            auto moveLeft = gameEngine_->checkTransportMovement(oldCubePos, newCubePos, transportId, moves);

            // if invalid movement
            if (moveLeft < 0)
            {
                canMove = false;
            }

            // if move is valid
            else
            {
                // move backend in stage 1
                if (gameState_->currentGamePhase()==1)
                {
                    gameEngine_->moveTransport(oldCubePos, newCubePos, transportId);
                    emit updateMovementLeft();
                }

                // move backend in stage 3
                else
                {
                    gameEngine_->moveTransportWithSpinner(oldCubePos, newCubePos, transportId, wheelOutput_.second);
                }

                moveGraphicTransport(transportId, oldCubePos, newCubePos);

                // move graphic pawns
                for (auto pawn : graphicTransport->getPawnList())
                {
                    pawn->setPos(pawn->scenePos() + newPos - oldPos);
                    moveGraphicPawn(pawn->getPawn()->getId(), oldCubePos, newCubePos);
                }

                // perform action of actors in new hex
                if (graphicTransport->getTransport()->getTransportType().compare("boat")==0)
                {
                    auto data = dataMap_[cubeToString(newCubePos)];
                    for (auto actor : data.actors)
                    {
                        if (actor->getActor()->getActorType().compare("seamunster")==0)
                        {
                            doSeamunsterAction(actor->getActor()->getId());
                        }

                        else if (actor->getActor()->getActorType().compare("kraken")==0)
                        {
                            doKrakenAction(actor->getActor()->getId());
                        }
                    }

                }

                if (graphicTransport->getTransport()->getTransportType().compare("dolphin")==0)
                {
                    auto data = dataMap_[cubeToString(newCubePos)];
                    for (auto actor : data.actors)
                    {
                        if (actor->getActor()->getActorType().compare("shark")==0)
                        {
                            doSharkAction(actor->getActor()->getId());
                        }
                    }

                }

                // if stage 1 and no move left
                if (gameState_->currentGamePhase()==1 && moveLeft==0)
                {
                    changeStage(2);
                }

                if (gameState_->currentGamePhase()==3)
                {
                    changeStage(1);
                }
            }
        }
        else
        {
            canMove = false;
        }

    }

    // if cannot move, set to old position, else update existing player
    if (!canMove)
    {
        graphicTransportList_[transportId]->setPos(oldPos);
    }
    else
    {
        updateExistingPlayer();
    }
}

void HexBoard::addPawn(int pawnId, std::shared_ptr<Common::Pawn> pawnPtr)
// add graphic pawn to hexBoard
{
    auto hexCoord = pawnPtr->getCoordinates();
    auto pawnPos = generatePawnPosition(hexCoord);
    auto owner = gameState_->getPlayerName(pawnPtr->getPlayerId());
    auto pawnColor = getPawnColor(owner);

    auto graphicPawn = new UI::GraphicPawn(pawnPtr, boardScale_, pawnColor, owner);

    scene_->addItem(graphicPawn);
    graphicPawn->setPos(pawnPos.first*boardScale_, pawnPos.second*boardScale_);

    // add data using coordinate
    auto mapKey = cubeToString(hexCoord);
    if (dataMap_.find(mapKey) == dataMap_.end())
    {
        HexData data;
        data.coord = hexCoord;
        data.pawns.push_back(graphicPawn);
        dataMap_.insert(std::make_pair(mapKey, data));
    }
    else
    {
        dataMap_[mapKey].pawns.push_back(graphicPawn);
    }

    graphicPawnList_.insert(pawnId, graphicPawn);

    // connect signals
    connect(graphicPawn, SIGNAL(pawnIsMoved(int, QPointF, QPointF)), this, SLOT(pawnIsMoved(int, QPointF, QPointF)));
    connect(this, SIGNAL(setPawnMovement(bool, std::list<int>)), graphicPawn, SLOT(allowMovement(bool, std::list<int>)));
}

void HexBoard::addActor(int actor_id, std::shared_ptr<Common::Actor> actorPtr)
// add graphic actor to hexBoard
{
    auto hexCoord = actorPtr->getHex()->getCoordinates();
    auto actorPos = cubeToHexPos(hexCoord);

    auto graphicActor = new UI::GraphicActor(actorPtr);
    scene_->addItem(graphicActor);
    graphicActor->show();
    graphicActor->setScale(boardScale_*ACTOR_SCALE);
    actorPos.setX(actorPos.rx() + boardScale_);
    actorPos.setY(actorPos.ry() + boardScale_);

    graphicActor->setPos(actorPos);

    actorPos = graphicActor->scenePos();

    // add data using coordinate
    auto mapKey = cubeToString(hexCoord);
    if (dataMap_.find(mapKey) == dataMap_.end())
    {
        HexData data;
        data.coord = hexCoord;
        data.actors.push_back(graphicActor);
        dataMap_.insert(std::make_pair(mapKey, data));
    }
    else
    {
        dataMap_[mapKey].actors.push_back(graphicActor);
    }

    graphicActorList_.insert(actor_id, graphicActor);

    // connect signal
    connect(graphicActor, SIGNAL(actorIsMoved(int, QPointF, QPointF)), this, SLOT(actorIsMoved(int, QPointF, QPointF)));
    connect(this, SIGNAL(setActorMovement(bool, std::list<int>)), graphicActor, SLOT(allowMovement(bool, std::list<int>)));
    update();
}

void HexBoard::addTransport(int transportId, std::shared_ptr<Common::Transport> transportPtr)
// add graphic transport
{
    auto hexCoord = transportPtr->getHex()->getCoordinates();
    auto transportPos = cubeToHexPos(hexCoord);

    auto graphicTransport = new UI::GraphicTransport(transportPtr, boardScale_);
    scene_->addItem(graphicTransport);
    graphicTransport->setPos(transportPos);

    // add data using coorindate
    auto mapKey = cubeToString(hexCoord);
    if (dataMap_.find(mapKey) == dataMap_.end())
    {
        HexData data;
        data.coord = hexCoord;
        data.transports.push_back(graphicTransport);
        dataMap_.insert(std::make_pair(mapKey, data));
    }
    else
    {
        dataMap_[mapKey].transports.push_back(graphicTransport);
    }

    graphicTransportList_.insert(transportId, graphicTransport);


    connect(graphicTransport, SIGNAL(transportIsMoved(int, QPointF, QPointF)), this, SLOT(transportIsMoved(int, QPointF, QPointF)));
    connect(this, SIGNAL(setTransportMovement(bool)), graphicTransport, SLOT(allowMovement(bool)));
}


void HexBoard::addHex(std::shared_ptr<Common::Hex> hexPtr, int id)
// add graphic hex to scene
{
    // add hex centers to list
    hexCenters.push_back(std::make_pair(cubeToHexCenter(hexPtr->getCoordinates()), hexPtr->getCoordinates()));

    // calculate vertex coordinates
    QPointF pos = cubeToHexPos(hexPtr->getCoordinates());

    QVector<QPointF> vertices;
    vertices << QPointF(3.0, 0.0)*boardScale_;
    vertices << QPointF(6.0, 2.0)*boardScale_;
    vertices << QPointF(6.0, 5.0)*boardScale_;
    vertices << QPointF(3.0, 7.0)*boardScale_;
    vertices << QPointF(0.0, 5.0)*boardScale_;
    vertices << QPointF(0.0, 2.0)*boardScale_;

    // create graphic_hex object
    UI::GraphicHex* graphicHex = new UI::GraphicHex(hexPtr, vertices, id);

    // add graphic_hex to scene
    scene_->addItem(graphicHex);
    graphicHex->setPos(pos);

    // add to list of graphic hex
    graphicHexList.insert(id, graphicHex);

    connect(graphicHex, SIGNAL(clicked(int)), this, SLOT(hexClicked(int)));
}


void HexBoard::populate()
// populate graphic items on hexboard
{
    // add hex
    int hexId = 0;
    for (auto it: *(gameBoard_->getHexList()))
    {
        addHex(it.second, hexId);
        hexId += 1;
    }

    // add boats
    for (auto it : *(gameBoard_->getTransportList()))
    {
        int transportId = it.first;
        auto transportPtr = it.second.first;
        addTransport(transportId, transportPtr);
    }

    // add pawns
    for (auto it: *(gameBoard_->getPawnList()))
    {
        int pawnId = it.first;
        auto pawnPtr = it.second.first;
        addPawn(pawnId, pawnPtr);
    }

    enablePawnMovement();
}

double HexBoard::euclideanDistance(std::pair<double, double> x, std::pair<double, double> y)
// calculate square euclidean distance
{
    return std::pow(x.first - y.first, 2) + std::pow(x.second - y.second, 2);
}


std::pair<double, double> HexBoard::cubeToHexCenter(Common::CubeCoordinate coord)
// convert cube coordinate to 2D coordinate of hex center, without scaling factor
{
    double x, y;
    x = 3*coord.x - 3*coord.y + 67;
    y = 5*coord.x + 5*coord.y + 76.5;
    return std::make_pair(x,y);
}

Common::CubeCoordinate HexBoard::planeToCube(double x, double y)
// convert 2D coordinate to cube coorindate
{
    Common::CubeCoordinate outputPos;
    auto pos = std::make_pair(x,y);
    double minDis = std::numeric_limits<double>::max();

    for (auto center : hexCenters)
    {
        auto dist = euclideanDistance(pos, center.first);
        if (dist < minDis)
        {
            minDis = dist;
            outputPos = center.second;
        }
    }

    if (minDis > 9)
    {
        outputPos = Common::CubeCoordinate(-1, -1, -1);
    }
    return outputPos;
}

QPointF HexBoard::cubeToHexPos(Common::CubeCoordinate coord)
// calculate hex coorindate on scene based on cube coordinate
{
    auto center = cubeToHexCenter(coord);
    QPointF pos = QPointF((center.first - 3.0)*boardScale_, (center.second-3.5)*boardScale_);
    return pos;
}


std::pair<double, double> HexBoard::generatePawnPosition(Common::CubeCoordinate coord)
// generate initial pawn position on mountain tile
// maximum 4 pawns are put on 1 mountain tile
{
    auto hexCenter = cubeToHexCenter(coord);

    QString key = QString::number(coord.x) + QString::number(coord.y);
    // if no key exist -> 1st pawn in the hex
    if (pawnVertices_.find(key) == pawnVertices_.end())
    {
        pawnVertices_[key] = 1;
        auto pawnCenter = std::make_pair(hexCenter.first-2.5, hexCenter.second-0.5);
        return pawnCenter;
    }
    else
    {
        int noExistingPawn = pawnVertices_[key];
        if (noExistingPawn == 1)
        {
            pawnVertices_[key] += 1;
            auto pawnCenter = std::make_pair(hexCenter.first-1.5, hexCenter.second-2.0);
            return pawnCenter;
        }
        else if (noExistingPawn == 2)
        {
            pawnVertices_[key] += 1;
            auto pawnCenter = std::make_pair(hexCenter.first+0.5, hexCenter.second-2.0);
            return pawnCenter;
        }
        else
        {
            pawnVertices_[key] += 1;
            auto pawnCenter = std::make_pair(hexCenter.first+1.5, hexCenter.second-0.5);
            return pawnCenter;
        }
    }
}


QColor HexBoard::getPawnColor(std::string pawnOwner)
// get pawn color according to owner
{
    return pawnColors_[QString::fromUtf8(pawnOwner.c_str())];
}

bool HexBoard::isPawnOnTransport(QPointF pawnPos, UI::GraphicTransport *transport)
// check if pawn is on transport
{
    double pawnCenterX = pawnPos.rx()/boardScale_ + 0.5;
    double pawnCenterY = pawnPos.ry()/boardScale_ + 0.5;

    if (transport->getTransport()->getTransportType().compare("boat")==0)
    {
        double xMin = transport->scenePos().rx()/boardScale_ + 1.0;
        double xMax = transport->scenePos().rx()/boardScale_ + 5.0;
        double yMin = transport->scenePos().ry()/boardScale_ + 3.5;
        double yMax = transport->scenePos().ry()/boardScale_ + 5.0;

        if (pawnCenterX > xMin && pawnCenterX < xMax && pawnCenterY > yMin && pawnCenterY < yMax)
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
        double xMin = transport->scenePos().rx()/boardScale_ + 2.5;
        double xMax = transport->scenePos().rx()/boardScale_ + 4.5;
        double yMin = transport->scenePos().ry()/boardScale_ + 4.0;
        double yMax = transport->scenePos().ry()/boardScale_ + 5.5;

        if (pawnCenterX > xMin && pawnCenterX < xMax && pawnCenterY > yMin && pawnCenterY < yMax)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void HexBoard::changePlayerTurn(int playerId)
// change the turn to next player, given the current player's id
{
    // find player index
    unsigned int playerIndex=0;
    for (unsigned int i=0; i< players.size(); i++)
    {
        if (players[i]->getPlayerId() == playerId)
        {
            playerIndex = i;
            break;
        }
    }

    // change to index of next player
    if (playerIndex == players.size()-1)
    {
        playerIndex = 0;
    }
    else
    {
        playerIndex += 1;
    }

    gameState_->changePlayerTurn(players[playerIndex]->getPlayerId());
    emit updatePlayerTurn();
    gameState_->setActionsLeft(3);
    emit updateMovementLeft();
}

void HexBoard::updateExistingPlayer()
// update list of survived players
{
    std::vector<unsigned int> removedPlayers;

    // find player who has no pawn left
    for (unsigned int i=0; i < players.size(); i++)
    {
        auto player = players[i];
        int pawnLeft = 0;
        for (auto pawn : *(gameBoard_->getPawnList()))
        {
            if (player->getPlayerId() == pawn.second.first->getPlayerId())
            {
                pawnLeft++;
            }
        }
        if (pawnLeft == 0)
        {
            removedPlayers.push_back(i);
        }
    }

    // remove player from vector of players
    for (auto i=0; i < removedPlayers.size(); i++)
    {
        players.erase(players.begin() + removedPlayers[i]-i);
    }

    // if only 1 player left, clear data and emit game over
    if (players.size()==1)
    {
        clear();
        emit gameOver(gameState_->getPlayerName(players[0]->getPlayerId()));
    }
    else if (players.size()==0)
    {
        clear();
        emit gameOver("");
    }
}

void HexBoard::doVortexAction(int id)
// destroy everything on current hex and neighboring hexes
{

    HexData data;
    auto graphicVortex = graphicActorList_[id];
    auto hex = graphicVortex->getActor()->getHex();
    auto neighbors = hex->getNeighbourVector();

    // destroy everything on current hex

    auto key = cubeToString(hex->getCoordinates());
    dataMap_[key] = removeData(key, true, "boat", "shark");
    dataMap_[key] = removeData(key, false, "dolphin", "kraken");
    dataMap_[key] = removeData(key, false, "", "seamunster");
    dataMap_[key] = removeData(key, false, "", "vortex");

    // destroy everything on neighboring water hexes
    for (auto neighborHex : neighbors)
    {
        // check if water hex, destroy thing
        if (gameBoard_->getHex(neighborHex)->isWaterTile())
        {
            auto key = cubeToString(neighborHex);
            dataMap_[key] = removeData(key, true, "boat", "shark");
            dataMap_[key] = removeData(key, false, "dolphin", "kraken");
            dataMap_[key] = removeData(key, false, "", "seamunster");
        }
    }

}

void HexBoard::doKrakenAction(int id)
/***
description: destroy boat on current hex, pawns are put on the sea again
***/
{

    auto kraken = graphicActorList_[id];
    auto coord = kraken->getActor()->getHex()->getCoordinates();

    // destroy graphical boats
    dataMap_[cubeToString(coord)] = removeData(cubeToString(coord), false, "boat", "");

}

void HexBoard::doSharkAction(int id)
/***
description: shark eats pawns and dolphin on hex

 ***/
{
    auto shark = graphicActorList_[id];
    auto coord = shark->getActor()->getHex()->getCoordinates();

    // destroy graphical and backend pawns
    dataMap_[cubeToString(coord)] = removeData(cubeToString(coord), true, "dolphin", "");
}

void HexBoard::doSeamunsterAction(int id)
/***
description: seamunster destroys boat and pawns
 ***/
{
    auto seamunster = graphicActorList_[id];
    auto coord = seamunster->getActor()->getHex()->getCoordinates();

    // destroy graphical and backend boats and pawns
    dataMap_[cubeToString(coord)] = removeData(cubeToString(coord), true, "boat", "");
}

std::string HexBoard::cubeToString(Common::CubeCoordinate coord)
// convert cube coordinate to string as key to hash table
{
    return std::to_string(coord.x) + "+" + std::to_string(coord.y) + "+" + std::to_string(coord.z);
}

HexData HexBoard::removeData(std::string key, bool pawn, std::string transportType, std::string actorType)
// perform both graphic and backend data remove with given flags
{
    auto data = dataMap_[key];
    // remove pawns
    if (pawn)
    {
        std::vector<int> IDs;
        std::vector<int> increment;

        for (auto pawnPtr : data.pawns)
        {
            // subtract 1 point of pawn's owner
            IDs.push_back(pawnPtr->getPawn()->getPlayerId());
            increment.push_back(-1);

            // add 1 point to current player if not destroy own pawn
            if (gameState_->currentPlayer() != pawnPtr->getPawn()->getPlayerId())
            {
                IDs.push_back(gameState_->currentPlayer());
                increment.push_back(1);
            }

            // remove pawn from transport on the same hex if pawn is on transport
            for (auto transport : data.transports)
            {
                if (transport->getTransport()->isPawnInTransport(pawnPtr->getPawn()))
                {
                    transport->getTransport()->removePawn(pawnPtr->getPawn());
                    transport->removePawn(pawnPtr);
                }
            }

            // remove pawn from hex
            gameBoard_->removePawn(pawnPtr->getPawn()->getId());

            // remove pawn from graphic_pawn_list
            auto it = graphicPawnList_.find(pawnPtr->getPawn()->getId());
            if (it != graphicPawnList_.end())
            {
                graphicPawnList_.erase(it);
            }


            // remove graphic pawn from scene
            scene_->removeItem(pawnPtr);

            delete pawnPtr;
        }
        // clear pawns from data
        data.pawns.clear();

        if (IDs.size() !=0)
        {
            emit(updatePoint(IDs, increment));
        }
    }

    // remove transport
    if (transportType.compare("")!=0)
    {
        std::vector<unsigned int> indices;
        unsigned int increment = 0;
        for (auto transportPtr : data.transports)
        {
            // if matching the type
            if (transportPtr->getTransport()->getTransportType().compare(transportType)==0)
            {

                // remove pawn from graphic_transport_list
                auto it = graphicTransportList_.find(transportPtr->getTransport()->getId());
                if (it != graphicTransportList_.end())
                {
                    graphicTransportList_.erase(it);
                }

                // remove transport from hex
                gameBoard_->removeTransport(transportPtr->getTransport()->getId());

                // remove graphic transport from scene
                scene_->removeItem(transportPtr);

                // free memory of graphic transport
                delete transportPtr;

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
    if (actorType.compare("")!=0)
    {
        std::vector<unsigned int> indices;
        unsigned int increment = 0;

        for (auto actorPtr : data.actors)
        {
            // if matching the type
            if (actorPtr->getActor()->getActorType().compare(actorType)==0)
            {

                // remove pawn from graphic_transport_list
                auto it = graphicActorList_.find(actorPtr->getActor()->getId());
                if (it != graphicActorList_.end())
                {
                    graphicActorList_.erase(it);
                }

                // remove transport from hex
                gameBoard_->removeActor(actorPtr->getActor()->getId());

                // remove graphic transport from scene
                scene_->removeItem(actorPtr);

                // free memory of graphic transport
                delete actorPtr;

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

void HexBoard::enablePawnMovement()
// enable movement of pawns in currentPlayerPawnList_
{
    // activate pawns of current player
    for (auto pawnIt=graphicPawnList_.begin(); pawnIt!=graphicPawnList_.end(); pawnIt++)
    {
        if (pawnIt.value()->getPawn()->getPlayerId() == gameState_->currentPlayer())
        {
            currentPlayerPawnList_.push_back(pawnIt.key());
        }
    }

    emit(setPawnMovement(true, currentPlayerPawnList_));
}

void HexBoard::enableTransportMovement()
// enable movement of transports in movableTransports_
{
    for (auto transportIt : graphicTransportList_)
    {
        for (auto pawnId : currentPlayerPawnList_)
        {
            if (transportIt->getTransport()->isPawnInTransport(graphicPawnList_[pawnId]->getPawn()))
            {
                movableTransports_.push_back(transportIt->getTransport()->getId());
                transportIt->allowMovement(true);
            }
        }
    }
}



void HexBoard::disablePawnMovement()
// disbale movement of pawns in currentPlayerPawnList_
{
    emit(setPawnMovement(false, currentPlayerPawnList_));
    currentPlayerPawnList_.clear();
}

void HexBoard::disableTransportMovement()
// disable movement of transport in movableTransports_
{
    for (auto id : movableTransports_)
    {
        graphicTransportList_[id]->allowMovement(false);
    }
    movableTransports_.clear();
}

void HexBoard::changeStage(int stage)
// change to corresponding stage and enable/disable corresponding items
{
    if (stage == 1)
    {
        // clear current player's pawns
        currentPlayerPawnList_.clear();

        // change to next player
        changePlayerTurn(gameEngine_->currentPlayer());
        emit updatePlayerTurn();

        // enable pawn movement
        enablePawnMovement();

        // change stage
        gameState_->changeGamePhase(Common::GamePhase(1));
        emit updateStage();

        // enable transport movement
        enableTransportMovement();

        // disable wheel output
    }

    if (stage == 2)
    {
        disableTransportMovement();
        // if island is not sunk
        if (!gameBoard_->isIslandSunk())
        {
            gameState_->changeGamePhase(Common::GamePhase(2));
            emit updateStage();
            disablePawnMovement();

        }

        // if island is sunk
        else
        {
            gameState_->changeGamePhase(Common::GamePhase(3));
            emit updateStage();

            disablePawnMovement();
            emit setWheelClick(true);
        }
    }

    if (stage == 3)
    {
        gameState_->changeGamePhase(Common::GamePhase(3));
        emit updateStage();
        emit setWheelClick(true);
    }
}


bool HexBoard::checkActorMovement(Common::CubeCoordinate newPos, int id)
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
    auto graphicActor = graphicActorList_[id];

    if (graphicActor->getActor()->getActorType().compare("shark")==0)
    {
        auto target_data = dataMap_[cubeToString(newPos)];
        for (auto actor : target_data.actors)
        {
            if (actor->getActor()->getActorType().compare("seamunster")==0)
            {
                allowed = false;
                return allowed;
            }
        }

        for (auto transport : target_data.transports)
        {
            if (transport->getTransport()->getTransportType().compare("boat")==0)
            {
                allowed = false;
                return allowed;
            }
        }
    }

    if (graphicActor->getActor()->getActorType().compare("kraken")==0)
    {
        auto target_data = dataMap_[cubeToString(newPos)];
        for (auto actor : target_data.actors)
        {
            if (actor->getActor()->getActorType().compare("seamunster")==0)
            {
                allowed = false;
                return allowed;
            }
        }
    }

    if (graphicActor->getActor()->getActorType().compare("seamunster")==0)
    {
        auto target_data = dataMap_[cubeToString(newPos)];
        for (auto actor : target_data.actors)
        {
            if (actor->getActor()->getActorType().compare("shark")==0)
            {
                allowed = false;
                return allowed;
            }
        }
    }

    return allowed;
}

bool HexBoard::checkTransportMovement(Common::CubeCoordinate oldPos, Common::CubeCoordinate newPos, int id)
/***
Description: check transport movement, add additional constraint to check movement

LOGIC
-----
if exist transport in target hex, movement is not allowed
 ***/
{
    bool allowed = true;
    auto targetData = dataMap_[cubeToString(newPos)];
    if (targetData.transports.size() > 0)
    {
        allowed = false;
    }

    return allowed;
}

void HexBoard::moveGraphicPawn(int pawnId, Common::CubeCoordinate oldPos, Common::CubeCoordinate newPos)
// remove graphic pawn in old dataMap position and move to new dataMap position
{
    auto oldData = dataMap_[cubeToString(oldPos)];
    auto newData = dataMap_[cubeToString(newPos)];

    int index = 0;
    for (auto it : oldData.pawns)
    {
        if (it->getPawn() == graphicPawnList_[pawnId]->getPawn())
        {
            break;
        }
        index++;
    }

    auto it = oldData.pawns.begin();
    advance(it, index);
    oldData.pawns.erase(it);
    dataMap_[cubeToString(oldPos)] = oldData;

    dataMap_[cubeToString(newPos)].pawns.push_back(graphicPawnList_[pawnId]);
}

void HexBoard::moveGraphicTransport(int transportId, Common::CubeCoordinate oldPos, Common::CubeCoordinate newPos)
// remove graphic transport in old dataMap position and move to new position
{
    int index = 0;
    for (auto it : dataMap_[cubeToString(oldPos)].transports)
    {
        if (it->getTransport()->getId() == graphicTransportList_[transportId]->getTransport()->getId())
        {
            break;
        }
        index++;
    }

    auto it = dataMap_[cubeToString(oldPos)].transports.begin();
    advance(it, index);
    dataMap_[cubeToString(oldPos)].transports.erase(it);
    dataMap_[cubeToString(newPos)].transports.push_back(graphicTransportList_[transportId]);
}

void HexBoard::moveGraphicActor(int actorId, Common::CubeCoordinate oldPos, Common::CubeCoordinate newPos)
// remove graphic actor from old position in dataMap and move to new position
{
    auto oldData = dataMap_[cubeToString(oldPos)];
    auto newData = dataMap_[cubeToString(newPos)];

    int index = 0;
    for (auto it : oldData.actors)
    {
        if (it->getActor()->getId() == graphicActorList_[actorId]->getActor()->getId())
        {
            break;
        }
        index++;
    }

    auto it = oldData.actors.begin();
    advance(it, index);
    oldData.actors.erase(it);
    dataMap_[cubeToString(oldPos)] = oldData;

    newData.actors.push_back(graphicActorList_[actorId]);

    dataMap_[cubeToString(newPos)] = newData;

    return;
}

void HexBoard::wheelEvent(QWheelEvent *event)
// handle wheel scrolling
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    wheelSchedule_ += numSteps;
    if (wheelSchedule_ * numSteps < 0)
    wheelSchedule_ = numSteps;

    QTimeLine *anim = new QTimeLine(350, this);
    anim->setUpdateInterval(20);

    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    connect(anim, SIGNAL (finished()), SLOT (animFinished()));
    anim->start();
}

void HexBoard::scalingTime(qreal x)
{
    qreal factor = 1.0+ qreal(wheelSchedule_) / 300.0;
    this->scale(factor, factor);
}

void HexBoard::animFinished()
{
    if (wheelSchedule_ > 0)
    wheelSchedule_--;
    else
    wheelSchedule_++;
    sender()->~QObject();
}
}


