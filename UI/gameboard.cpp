#include<gameboard.hh>
#include<pawn.hh>
#include<hex.hh>
#include<iostream>


GameBoard::~GameBoard()
{
    GameBoard::hex_list.clear();
    GameBoard::pawn_list.clear();
    return;
}
int GameBoard::checkTileOccupation(Common::CubeCoordinate tileCoord) const
{
    auto hex_ptr = GameBoard::getHex(tileCoord);
    return hex_ptr->getPawnAmount();
}

bool GameBoard::isWaterTile(Common::CubeCoordinate tileCoord) const
{
    auto hex_ptr = GameBoard::getHex(tileCoord);
    if (hex_ptr->getPieceType().compare("Water")==0)
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
    auto hex_coord = newHex->getCoordinates();
    std::string hex_key = GameBoard::coordToString(hex_coord);
    GameBoard::hex_list[hex_key] = newHex;
}

std::shared_ptr<Common::Hex> GameBoard::getHex(Common::CubeCoordinate hexCoord) const
{
    std::string hex_key = GameBoard::coordToString(hexCoord);

    if (GameBoard::hex_list.find(hex_key)!=GameBoard::hex_list.end())
    {
        auto hex_ptr = GameBoard::hex_list.at(hex_key);
        return hex_ptr;
    }
    else
    {
        return nullptr;
    }
}

void GameBoard::addPawn(int playerId, int pawnId)
{
    std::shared_ptr<Common::Pawn> pawn_ptr = std::make_shared<Common::Pawn>();
    Common::CubeCoordinate pawn_coord = Common::CubeCoordinate(0,0,0);
    pawn_ptr->setCoordinates(pawn_coord);
    pawn_ptr->setId(pawnId, playerId);
    auto hex_ptr = GameBoard::getHex(pawn_coord);
    hex_ptr->addPawn(pawn_ptr);
    GameBoard::pawn_list[pawnId] = std::make_pair(pawn_ptr, hex_ptr);
}

void GameBoard::removePawn(int pawnId)
{
    auto pawn_it = GameBoard::pawn_list.find(pawnId);
    auto pawn_ptr = pawn_it->second.first;
    auto hex_ptr = pawn_it->second.second;
    hex_ptr->removePawn(pawn_ptr);
    GameBoard::pawn_list.erase(pawn_it);
}

void GameBoard::movePawn(int pawnId, Common::CubeCoordinate pawnCoord)
{
    auto pawn_data = GameBoard::pawn_list.at(pawnId);
    auto pawn_ptr = pawn_data.first;
    auto hex_ptr = pawn_data.second;
    pawn_ptr->setCoordinates(pawnCoord);
    hex_ptr->removePawn(pawn_ptr);
    auto new_hex_ptr = GameBoard::getHex(pawnCoord);
    new_hex_ptr->addPawn(pawn_ptr);
}

std::shared_ptr<Common::Hex> GameBoard::findActor(int actorId) const
{
    Common::CubeCoordinate coord;

    for (auto it=GameBoard::hex_list.begin(); it!= GameBoard::hex_list.end(); it++)
    {

        if (it->second->giveActor(actorId) != nullptr)
        {
            coord = it->second->getCoordinates();
            break;
        }
    }

    auto hex_ptr = GameBoard::hex_list.at(GameBoard::coordToString(coord));

    return hex_ptr;
}

void GameBoard::moveActor(int actorId, Common::CubeCoordinate actorCoord)
{
    auto old_hex_ptr = GameBoard::findActor(actorId);
    auto new_hex_ptr = GameBoard::hex_list.at(GameBoard::coordToString(actorCoord));

    auto actor_ptr = old_hex_ptr->giveActor(actorId);
    old_hex_ptr->removeActor(actor_ptr);
    new_hex_ptr->addActor(actor_ptr);
}

void GameBoard::removeActor(int actorId)
{
    auto hex_ptr = GameBoard::findActor(actorId);
    auto actor_ptr = hex_ptr->giveActor(actorId);
    hex_ptr->removeActor(actor_ptr);
}

std::string GameBoard::coordToString(Common::CubeCoordinate coord) const
{
    std::string key = std::to_string(coord.x) + "+" + std::to_string(coord.y) + "+" + std::to_string(coord.z);
    return key;
}

std::pair<int,int> GameBoard::CubeToRegularCoordinate(Common::CubeCoordinate coord)
{
    int x,y;
    x = coord.x + coord.y -5;
    y = coord.y - coord.x -7;
    return std::make_pair(x,y);
}

void GameBoard::print_hex_list() const
{
    for (auto it=GameBoard::hex_list.begin(); it!=GameBoard::hex_list.end(); it++)
    {
        auto coord = it->second->getCoordinates();
        auto type = it->second->getPieceType();
        std::cerr<< "coordinate: " << coordToString(coord) << "\n";
        std::cerr<< "type: " << type << "\n";
    }
}

void GameBoard::print_hex_stat() const
{
    int min_x = 10000;
    int max_x = -10000;
    int min_y = 10000;
    int max_y = -10000;
    int min_z = 10000;
    int max_z = -10000;

    int count = 0;
    for (auto it=GameBoard::hex_list.begin(); it!=GameBoard::hex_list.end(); it++)
    {
        auto coord = it->second->getCoordinates();
        count += 1;
        if (coord.x < min_x)
        {
            min_x = coord.x;
        }
        if (coord.x > max_x)
        {
            max_x = coord.x;
        }

        if (coord.y < min_y)
        {
            min_y = coord.y;
        }
        if (coord.y > max_y)
        {
            max_y = coord.y;
        }


        if (coord.z < min_z)
        {
            min_z = coord.z;
        }
        if (coord.z > max_z)
        {
            max_z = coord.z;
        }


    }

    std::cerr<<"number of tiles: " << std::to_string(count);
    std::cerr<<"min x: " << std::to_string(min_x) << ", max x: " << std::to_string(max_x) << "\n";
    std::cerr<<"min y: " << std::to_string(min_y) << ", max y: " << std::to_string(max_y) << "\n";
    std::cerr<<"min z: " << std::to_string(min_z) << ", max z: " << std::to_string(max_z) << "\n";
}
