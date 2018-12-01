#include<gameboard.hh>
#include<pawn.hh>
#include<hex.hh>
#include<iostream>
#include<actor.hh>
#include<transport.hh>

namespace Student {

GameBoard::GameBoard()
{
    hexList_ = std::make_shared<std::unordered_map<std::string, std::shared_ptr<Common::Hex>>>();
    pawnList_ = std::make_shared<std::unordered_map<int, std::pair<std::shared_ptr<Common::Pawn>, std::shared_ptr<Common::Hex>>>>();
    actorList_ = std::make_shared<std::unordered_map<int, std::pair<std::shared_ptr<Common::Actor>, std::shared_ptr<Common::Hex>>>>();
    transportList_ = std::make_shared<std::unordered_map<int, std::pair<std::shared_ptr<Common::Transport>, std::shared_ptr<Common::Hex>>>>();
}

GameBoard::~GameBoard()
{
    GameBoard::hexList_->clear();
    GameBoard::pawnList_->clear();
    GameBoard::transportList_->clear();
    GameBoard::actorList_->clear();
    return;
}
int GameBoard::checkTileOccupation(Common::CubeCoordinate tileCoord) const
{
    auto hexPtr = GameBoard::getHex(tileCoord);
    if (hexPtr == nullptr) {
        return -1;
    }
    return hexPtr->getPawnAmount();
}

bool GameBoard::isWaterTile(Common::CubeCoordinate tileCoord) const
{
    auto hexPtr = GameBoard::getHex(tileCoord);
//    if (hex_ptr->getPieceType().compare("Water")==0)
    if (hexPtr->getPieceType() == "Water")
    {
        return true;
    }
    else
    {
        return false;
    }
}

void GameBoard::addHex(std::shared_ptr<Common::Hex> newHex)
{
    auto hexCoord = newHex->getCoordinates();
    std::string hexKey = GameBoard::coordToString(hexCoord);
    // find if the hex is already there in the game board
    if (getHex(hexCoord) != nullptr) {
        auto iter = GameBoard::hexList_->find(hexKey);
        GameBoard::hexList_->erase(iter);
    }
    GameBoard::hexList_->insert(std::make_pair(hexKey, newHex));
}

std::shared_ptr<Common::Hex> GameBoard::getHex(Common::CubeCoordinate hexCoord) const
{
    std::string hexKey = GameBoard::coordToString(hexCoord);

    if (GameBoard::hexList_->find(hexKey)!=GameBoard::hexList_->end())
    {
        auto hexPtr = GameBoard::hexList_->at(hexKey);
        return hexPtr;
    }
    else
    {
        return nullptr;
    }
}

void GameBoard::addPawn(int playerId, int pawnId)
{
    Common::CubeCoordinate pawnCoord = Common::CubeCoordinate(0,0,0);
    std::shared_ptr<Common::Pawn> pawnPtr = std::make_shared<Common::Pawn>(pawnId, playerId, pawnCoord);
    auto hexPtr = GameBoard::getHex(pawnCoord);
    hexPtr->addPawn(pawnPtr);
    Student::GameBoard::pawnList_->insert(std::make_pair(pawnId, std::make_pair(pawnPtr, hexPtr)));
}

void GameBoard::addPawn(int playerId, int pawnId, Common::CubeCoordinate coord)
{
    std::shared_ptr<Common::Pawn> pawnPtr = std::make_shared<Common::Pawn>(pawnId, playerId, coord);
    auto hexPtr = GameBoard::getHex(coord);
    hexPtr->addPawn(pawnPtr);
    GameBoard::pawnList_->insert(std::make_pair(pawnId, std::make_pair(pawnPtr, hexPtr)));
}

void GameBoard::removePawn(int pawnId)
{
    auto pawnIt = GameBoard::pawnList_->find(pawnId);
    auto pawnPtr = pawnIt->second.first;
    auto hexPtr = pawnIt->second.second;
    hexPtr->removePawn(pawnPtr);
    GameBoard::pawnList_->erase(pawnIt);
}



void GameBoard::movePawn(int pawnId, Common::CubeCoordinate pawnCoord)
{
    auto pawnData = GameBoard::pawnList_->at(pawnId);
    auto findHexPtr = GameBoard::hexList_->find(GameBoard::coordToString(pawnCoord));
    if (findHexPtr == GameBoard::hexList_->end()) {
        // cannot find the hex (tile) for the new actorCoord
        return; // actor stays in its place if the location doesn't exist
    }
    auto pawnPtr = pawnData.first;
    auto hexPtr = pawnData.second;

    pawnPtr->setCoordinates(pawnCoord);
    hexPtr->removePawn(pawnPtr);
    auto newHexPtr = GameBoard::getHex(pawnCoord);
    newHexPtr->addPawn(pawnPtr);
    (*pawnList_)[pawnId].second = newHexPtr;
}


void GameBoard::addActor(std::shared_ptr<Common::Actor> actor, Common::CubeCoordinate actorCoord)
{
    auto hexPtr = GameBoard::getHex(actorCoord);

    if (hexPtr->giveActor(actor->getId()) == nullptr)
    {
        hexPtr->addActor(actor);
    }

    if (actor->getHex()==nullptr)
    {
        actor->addHex(hexPtr);
    }

    GameBoard::actorList_->insert(std::make_pair(actor->getId(), std::make_pair(actor, hexPtr)));
}


void GameBoard::moveActor(int actorId, Common::CubeCoordinate actorCoord)
{
    auto actorData = GameBoard::actorList_->at(actorId);
    auto hexPtr = GameBoard::hexList_->find(GameBoard::coordToString(actorCoord));
    if (hexPtr == GameBoard::hexList_->end()) {
        // cannot find the hex (tile) for the new actorCoord
        return; // actor stays in its place if the location doesn't exist
    }

    auto newHexPtr = GameBoard::hexList_->at(GameBoard::coordToString(actorCoord));

    actorData.second->removeActor(actorData.first);
    newHexPtr->addActor(actorData.first);
    actorData.first->move(newHexPtr);

    (*actorList_)[actorId].second = newHexPtr;
}

void GameBoard::removeActor(int actorId)
{
    auto it = GameBoard::actorList_->find(actorId);
    it->second.second->removeActor(it->second.first);
    GameBoard::actorList_->erase(it);
}

void GameBoard::addTransport(std::shared_ptr<Common::Transport> transport, Common::CubeCoordinate transportCoord)
{
    auto hexPtr = GameBoard::getHex(transportCoord);

    if (hexPtr->giveTransport(transport->getId()) == nullptr)
    {
        hexPtr->addTransport(transport);
    }

    if (transport->getHex()==nullptr)
    {
        transport->addHex(hexPtr);
    }

    GameBoard::transportList_->insert(std::make_pair(transport->getId(), std::make_pair(transport, hexPtr)));
}


void GameBoard::moveTransport(int id, Common::CubeCoordinate transportCoord)
{
    auto transportData = GameBoard::transportList_->at(id);
    auto hexPtr = GameBoard::hexList_->find(GameBoard::coordToString(transportCoord));
    if (hexPtr == GameBoard::hexList_->end()) {
        // cannot find the hex (tile) for the new transport coord
        return; // transport stays in its place if the location doesn't exist
    }
    auto newHexPtr = GameBoard::hexList_->at(GameBoard::coordToString(transportCoord));

    transportData.second->removeTransport(transportData.first);
    newHexPtr->addTransport(transportData.first);
    transportData.first->move(newHexPtr);

    (*transportList_)[id].second = newHexPtr;

    // move pawns on transport
    for (auto pawn : transportData.first->getPawnsInTransport())
    {
        movePawn(pawn->getId(), transportCoord);
    }
}

void GameBoard::removeTransport(int id)
{
    auto it = GameBoard::transportList_->find(id);
    it->second.second->removeTransport(it->second.first);
    GameBoard::transportList_->erase(it);
}

std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Common::Hex> > > GameBoard::getHexList()
{
    return hexList_;
}

std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Pawn>, std::shared_ptr<Common::Hex> > > > GameBoard::getPawnList()
{
    return pawnList_;
}

std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Actor>, std::shared_ptr<Common::Hex> > > > GameBoard::getActorList()
{
    return actorList_;
}

std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Transport>, std::shared_ptr<Common::Hex> > > > GameBoard::getTransportList()
{
    return transportList_;
}

std::string GameBoard::coordToString(Common::CubeCoordinate coord) const
{
    std::string key = std::to_string(coord.x) + "+" + std::to_string(coord.y) + "+" + std::to_string(coord.z);
    return key;
}


bool GameBoard::isIslandSunk()
{
    bool isSunk = true;
    for (auto hex : *hexList_)
    {
        if (!hex.second->isWaterTile())
        {
            isSunk = false;
            break;
        }
    }
    return isSunk;
}
}
