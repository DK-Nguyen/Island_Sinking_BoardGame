#include<gamestate.hh>
#include <player.hh>
#include <vector>
#include <unordered_map>
#include <string>

GameState::~GameState()
{
    return;
}

GameState::GameState(Common::GamePhase current_phase,
                     int current_player_id,
                     std::vector<std::shared_ptr<Common::IPlayer>> players,
                     std::shared_ptr<std::unordered_map<int, std::string>> player_names,
                     std::shared_ptr<std::unordered_map<std::string, int>> points,
                     std::shared_ptr<std::vector<std::pair<std::string, int>>> top10)
{
    this->current_phase = current_phase;
    this->current_player_id = current_player_id;
    this->players_ptr = players;
    this->points = points;
    this->top10 = top10;
    this->player_names = player_names;
}

Common::GamePhase GameState::currentGamePhase() const
{
    return GameState::current_phase;
}

int GameState::currentPlayer() const
{
    return GameState::current_player_id;
}

std::string GameState::currentPlayerName() const
{
    return player_names->at(currentPlayer());
}

std::string GameState::get_player_name(int player_id)
{
    return  player_names->at(player_id);
}

QString GameState::currentGamePhaseName() const
{
    if (current_phase == 1)
    {
        return QString("movement");
    }
    else if (current_phase == 2)
    {
        return QString("sinking");
    }
    else
    {
        return QString("spinning");
    }
}


void GameState::changeGamePhase(Common::GamePhase nextPhase)
{
    GameState::current_phase = nextPhase;
}

void GameState::changePlayerTurn(int nextPlayer)
{
    GameState::current_player_id = nextPlayer;
}

unsigned int GameState::getActionsLeft() const
{
    unsigned int actions = 0;
    for (auto it : players_ptr)
    {
        if (it->getPlayerId() == currentPlayer())
        {
            actions = it->getActionsLeft();
            break;
        }
    }

    return actions;
}


