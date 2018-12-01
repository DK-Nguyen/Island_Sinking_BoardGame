#ifndef GAMEBOARD_HH
#define GAMEBOARD_HH

#include <map>
#include <unordered_map>

#include "igameboard.hh"

namespace Student {
class GameBoard : public Common::IGameBoard
{
public:
    GameBoard();
    ~GameBoard();
    int checkTileOccupation(Common::CubeCoordinate tileCoord) const;

    bool isWaterTile(Common::CubeCoordinate tileCoord) const;

    void addHex(std::shared_ptr<Common::Hex> newHex);

    std::shared_ptr<Common::Hex> getHex(Common::CubeCoordinate hexCoord) const;

    void addPawn(int playerId, int pawnId);

    void addPawn(int playerId, int pawnId, Common::CubeCoordinate coord);

    void movePawn(int pawnId, Common::CubeCoordinate pawnCoord);

    void removePawn(int pawnId);

    void addActor(std::shared_ptr<Common::Actor> actor, Common::CubeCoordinate actorCoord);

    void moveActor(int actorId, Common::CubeCoordinate actorCoord);

    void removeActor(int actorId);

    void addTransport(std::shared_ptr<Common::Transport> transport, Common::CubeCoordinate transportCoord);

    void moveTransport(int id, Common::CubeCoordinate transportCoord);

    void removeTransport(int id);

    bool isIslandSunk();

    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Common::Hex>>> getHexList();
    std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Pawn>, std::shared_ptr<Common::Hex>>>> getPawnList();
    std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Actor>, std::shared_ptr<Common::Hex>>>> getActorList();
    std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Transport>, std::shared_ptr<Common::Hex>>>> getTransportList();

private:

    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Common::Hex>>> hexList_;
    std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Pawn>, std::shared_ptr<Common::Hex>>>> pawnList_;
    std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Actor>, std::shared_ptr<Common::Hex>>>> actorList_;
    std::shared_ptr<std::unordered_map<int, std::pair<std::shared_ptr<Common::Transport>, std::shared_ptr<Common::Hex>>>> transportList_;

    std::string coordToString(Common::CubeCoordinate coord) const;
};
}
#endif // GAMEBOARD_HH
