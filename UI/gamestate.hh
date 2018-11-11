#ifndef GAMESTATE_HH
#define GAMESTATE_HH

#include "igamestate.hh"
#include <unordered_map>
#include <QString>
#include <player.hh>
#include <vector>

class GameState : public Common::IGameState
{
public:
    GameState(Common::GamePhase current_phase,
              int current_player_id,
              std::vector<std::shared_ptr<Common::IPlayer>> players,
              std::shared_ptr<std::unordered_map<int, std::string>> player_names,
              std::shared_ptr<std::unordered_map<std::string, int>> points,
              std::shared_ptr<std::vector<std::pair<std::string, int>>> top10);

    ~GameState();

    Common::GamePhase currentGamePhase() const;

    int currentPlayer() const;

    std::string currentPlayerName() const;

    std::string get_player_name(int player_id);

    QString currentGamePhaseName() const;

    void changeGamePhase(Common::GamePhase nextPhase);

    void changePlayerTurn(int nextPlayer);

    unsigned int getActionsLeft() const;

    std::shared_ptr<std::unordered_map<std::string, int>> points;
    std::shared_ptr<std::vector<std::pair<std::string, int>>> top10;

private:
    Common::GamePhase current_phase;
    int current_player_id;
    std::string current_player;
    std::vector<std::shared_ptr<Common::IPlayer>> players_ptr;
    std::shared_ptr<std::unordered_map<int, std::string>> player_names;

};

#endif // GAMESTATE_HH
