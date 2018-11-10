#ifndef GAMEBOARD_HH
#define GAMEBOARD_HH

#include <map>
#include <unordered_map>

#include "igameboard.hh"

class GameBoard : public Common::IGameBoard
{
public:
    GameBoard() = default;
    ~GameBoard();
    virtual int checkTileOccupation(Common::CubeCoordinate tileCoord) const;

    virtual bool isWaterTile(Common::CubeCoordinate tileCoord) const;

    virtual void addHex(std::shared_ptr<Common::Hex> newHex);

    virtual std::shared_ptr<Common::Hex> getHex(Common::CubeCoordinate hexCoord) const;

    virtual void addPawn(int playerId, int pawnId);

    virtual void movePawn(int pawnId, Common::CubeCoordinate pawnCoord);

    virtual void removePawn(int pawnId);

    virtual void addActor(std::shared_ptr<Common::Actor> actor, Common::CubeCoordinate actor_coord);

    virtual void moveActor(int actorId, Common::CubeCoordinate actorCoord);

    virtual void removeActor(int actorId);

    virtual void addTransport(std::shared_ptr<Common::Transport> transport, Common::CubeCoordinate transport_coord);

    virtual void moveTransport(int id, Common::CubeCoordinate transport_coord);

    virtual void removeTransport(int id);

    void print_hex_list() const;
    void print_hex_stat() const;

    std::unordered_map<std::string, std::shared_ptr<Common::Hex>> hex_list;
    std::unordered_map<int, std::pair<std::shared_ptr<Common::Pawn>, std::shared_ptr<Common::Hex>>> pawn_list;
    std::unordered_map<int, std::pair<std::shared_ptr<Common::Actor>, std::shared_ptr<Common::Hex>>> actor_list;
    std::unordered_map<int, std::pair<std::shared_ptr<Common::Transport>, std::shared_ptr<Common::Hex>>> transport_list;

private:

    std::string coordToString(Common::CubeCoordinate coord) const;
};
#endif // GAMEBOARD_HH
