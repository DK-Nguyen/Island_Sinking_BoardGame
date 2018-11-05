#ifndef GAMEBOARD_HH
#define GAMEBOARD_HH

#include <map>
#include <unordered_map>

#include "igameboard.hh"

class GameBoard : public Common::IGameBoard
{
public:
    GameBoard();
    ~GameBoard();

};
#endif // GAMEBOARD_HH
