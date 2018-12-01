#ifndef GAMESTATE_HH
#define GAMESTATE_HH

#include "igamestate.hh"
#include <unordered_map>
#include <QString>
#include <player.hh>
#include <vector>

namespace Student{
class GameState : public Common::IGameState
{
public:
    GameState(Common::GamePhase currentPhase,
              int currentPlayerId,
              std::vector<std::shared_ptr<Common::IPlayer>> players,
              std::shared_ptr<std::unordered_map<int, std::string>> playerNames,
              std::shared_ptr<std::unordered_map<std::string, int>> points_,
              std::shared_ptr<std::vector<std::pair<std::string, int>>> top10);

    ~GameState();

    Common::GamePhase currentGamePhase() const;

    int currentPlayer() const;

    std::string currentPlayerName() const;

    std::string getPlayerName(int playerId);

    QString currentGamePhaseName() const;

    void changeGamePhase(Common::GamePhase nextPhase);

    void changePlayerTurn(int nextPlayer);

    unsigned int getActionsLeft() const;
    void setActionsLeft(unsigned int amount);

    std::shared_ptr<std::unordered_map<std::string, int>> points_;
    std::shared_ptr<std::vector<std::pair<std::string, int>>> top10;

private:
    Common::GamePhase currentPhase_;
    int currentPlayerId_;
    std::string current_player;
    std::vector<std::shared_ptr<Common::IPlayer>> playersPtr_;
    std::shared_ptr<std::unordered_map<int, std::string>> playerNames_;

};
}
#endif // GAMESTATE_HH
