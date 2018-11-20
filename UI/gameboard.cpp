#include<gameboard.hh>
#include<pawn.hh>
#include<hex.hh>
#include<iostream>
#include<actor.hh>
#include<transport.hh>


Student::GameBoard::GameBoard()
{
    hex_list = std::make_shared<std::unordered_map<std::string, std::shared_ptr<Common::Hex>>>();
    pawn_list = std::make_shared<std::unordered_map<int, std::pair<std::shared_ptr<Common::Pawn>, std::shared_ptr<Common::Hex>>>>();
    actor_list = std::make_shared<std::unordered_map<int, std::pair<std::shared_ptr<Common::Actor>, std::shared_ptr<Common::Hex>>>>();
    transport_list = std::make_shared<std::unordered_map<int, std::pair<std::shared_ptr<Common::Transport>, std::shared_ptr<Common::Hex>>>>();
}

Student::GameBoard::~GameBoard()
{
    GameBoard::hex_list->clear();
    GameBoard::pawn_list->clear();
    GameBoard::transport_list->clear();
    GameBoard::actor_list->clear();
    return;
}
int Student::GameBoard::checkTileOccupation(Common::CubeCoordinate tileCoord) const
{
    auto hex_ptr = GameBoard::getHex(tileCoord);
    if (hex_ptr == nullptr) {
        return -1;
    }
    return hex_ptr->getPawnAmount();
}

bool Student::GameBoard::isWaterTile(Common::CubeCoordinate tileCoord) const
{
    auto hex_ptr = GameBoard::getHex(tileCoord);
//    if (hex_ptr->getPieceType().compare("Water")==0)
    if (hex_ptr->getPieceType() == "Water")
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Student::GameBoard::addHex(std::shared_ptr<Common::Hex> newHex)
{
    auto hex_coord = newHex->getCoordinates();
    std::string hex_key = GameBoard::coordToString(hex_coord);
    // find if the hex is already there in the game board
    if (getHex(hex_coord) != nullptr) {
        auto iter = GameBoard::hex_list->find(hex_key);
        GameBoard::hex_list->erase(iter);
    }
    GameBoard::hex_list->insert(std::make_pair(hex_key, newHex));
}

std::shared_ptr<Common::Hex> Student::GameBoard::getHex(Common::CubeCoordinate hexCoord) const
{
    std::string hex_key = GameBoard::coordToString(hexCoord);

    if (GameBoard::hex_list->find(hex_key)!=GameBoard::hex_list->end())
    {
        auto hex_ptr = GameBoard::hex_list->at(hex_key);
        return hex_ptr;
    }
    else
    {
        return nullptr;
    }
}

void Student::GameBoard::addPawn(int playerId, int pawnId)
{
    Common::CubeCoordinate pawn_coord = Common::CubeCoordinate(0,0,0);
    std::shared_ptr<Common::Pawn> pawn_ptr = std::make_shared<Common::Pawn>(pawnId, playerId, pawn_coord);
    auto hex_ptr = GameBoard::getHex(pawn_coord);
    hex_ptr->addPawn(pawn_ptr);
    Student::GameBoard::pawn_list->insert(std::make_pair(pawnId, std::make_pair(pawn_ptr, hex_ptr)));
}

void Student::GameBoard::addPawn(int playerId, int pawnId, Common::CubeCoordinate coord)
{
    std::shared_ptr<Common::Pawn> pawn_ptr = std::make_shared<Common::Pawn>(pawnId, playerId, coord);
    auto hex_ptr = GameBoard::getHex(coord);
    hex_ptr->addPawn(pawn_ptr);
    GameBoard::pawn_list->insert(std::make_pair(pawnId, std::make_pair(pawn_ptr, hex_ptr)));
}

void Student::GameBoard::removePawn(int pawnId)
{
    auto pawn_it = GameBoard::pawn_list->find(pawnId);
    auto pawn_ptr = pawn_it->second.first;
    auto hex_ptr = pawn_it->second.second;
    hex_ptr->removePawn(pawn_ptr);
    GameBoard::pawn_list->erase(pawn_it);
}



void Student::GameBoard::movePawn(int pawnId, Common::CubeCoordinate pawnCoord)
{
    auto pawn_data = GameBoard::pawn_list->at(pawnId);
    auto find_hex_ptr = GameBoard::hex_list->find(GameBoard::coordToString(pawnCoord));
    if (find_hex_ptr == GameBoard::hex_list->end()) {
        // cannot find the hex (tile) for the new actorCoord
        return; // actor stays in its place if the location doesn't exist
    }
    auto pawn_ptr = pawn_data.first;
    auto hex_ptr = pawn_data.second;
    pawn_ptr->setCoordinates(pawnCoord);
    hex_ptr->removePawn(pawn_ptr);
    auto new_hex_ptr = GameBoard::getHex(pawnCoord);
    new_hex_ptr->addPawn(pawn_ptr);
}


void Student::GameBoard::addActor(std::shared_ptr<Common::Actor> actor, Common::CubeCoordinate actor_coord)
{
    auto hex_ptr = GameBoard::getHex(actor_coord);

    if (hex_ptr->giveActor(actor->getId()) == nullptr)
    {
        hex_ptr->addActor(actor);
    }

    GameBoard::actor_list->insert(std::make_pair(actor->getId(), std::make_pair(actor, hex_ptr)));
}


void Student::GameBoard::moveActor(int actorId, Common::CubeCoordinate actorCoord)
{
    auto actor_data = GameBoard::actor_list->at(actorId);
    auto hex_ptr = GameBoard::hex_list->find(GameBoard::coordToString(actorCoord));
    if (hex_ptr == GameBoard::hex_list->end()) {
        // cannot find the hex (tile) for the new actorCoord
        return; // actor stays in its place if the location doesn't exist
    }

    auto new_hex_ptr = GameBoard::hex_list->at(GameBoard::coordToString(actorCoord));

    actor_data.second->removeActor(actor_data.first);
    new_hex_ptr->addActor(actor_data.first);
    actor_data.first->move(new_hex_ptr);
}

void Student::GameBoard::removeActor(int actorId)
{
    auto it = GameBoard::actor_list->find(actorId);
    it->second.second->removeActor(it->second.first);
    GameBoard::actor_list->erase(it);
}

void Student::GameBoard::addTransport(std::shared_ptr<Common::Transport> transport, Common::CubeCoordinate transport_coord)
{
    auto hex_ptr = GameBoard::getHex(transport_coord);

    if (hex_ptr->giveTransport(transport->getId()) == nullptr)
    {
        hex_ptr->addTransport(transport);
    }

    if (transport->getHex()==nullptr)
    {
        transport->addHex(hex_ptr);
    }

    GameBoard::transport_list->insert(std::make_pair(transport->getId(), std::make_pair(transport, hex_ptr)));
}


void Student::GameBoard::moveTransport(int id, Common::CubeCoordinate transport_coord)
{
    auto transport_data = GameBoard::transport_list->at(id);
    auto hex_ptr = GameBoard::hex_list->find(GameBoard::coordToString(transport_coord));
    if (hex_ptr == GameBoard::hex_list->end()) {
        // cannot find the hex (tile) for the new actorCoord
        return; // actor stays in its place if the location doesn't exist
    }
    auto new_hex_ptr = GameBoard::hex_list->at(GameBoard::coordToString(transport_coord));

    transport_data.second->removeTransport(transport_data.first);
    new_hex_ptr->addTransport(transport_data.first);
    transport_data.first->move(new_hex_ptr);
}

void Student::GameBoard::removeTransport(int id)
{
    auto it = GameBoard::transport_list->find(id);
    it->second.second->removeTransport(it->second.first);
    GameBoard::transport_list->erase(it);
}

std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Common::Hex> > > Student::GameBoard::getHexList()
{
    return hex_list;
}

std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Pawn>, std::shared_ptr<Common::Hex> > > > Student::GameBoard::getPawnList()
{
    return pawn_list;
}

std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Actor>, std::shared_ptr<Common::Hex> > > > Student::GameBoard::getActorList()
{
    return actor_list;
}

std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Transport>, std::shared_ptr<Common::Hex> > > > Student::GameBoard::getTransportList()
{
    return transport_list;
}

std::string Student::GameBoard::coordToString(Common::CubeCoordinate coord) const
{
    std::string key = std::to_string(coord.x) + "+" + std::to_string(coord.y) + "+" + std::to_string(coord.z);
    return key;
}




