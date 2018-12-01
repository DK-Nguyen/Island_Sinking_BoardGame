#include<gamestate.hh>
#include <player.hh>
#include <vector>
#include <unordered_map>
#include <string>


namespace Student{

GameState::~GameState()
{
    return;
}

GameState::GameState(Common::GamePhase currentPhase,
                     int currentPlayerId,
                     std::vector<std::shared_ptr<Common::IPlayer>> players,
                     std::shared_ptr<std::unordered_map<int, std::string>> playerNames,
                     std::shared_ptr<std::unordered_map<std::string, int>> points,
                     std::shared_ptr<std::vector<std::pair<std::string, int>>> top10)
{
    this->currentPhase_ = currentPhase;
    this->currentPlayerId_ = currentPlayerId;
    this->playersPtr_ = players;
    this->points_ = points;
    this->top10 = top10;
    this->playerNames_ = playerNames;
}

Common::GamePhase GameState::currentGamePhase() const
{
    return Student::GameState::currentPhase_;
}

int GameState::currentPlayer() const
{
    return Student::GameState::currentPlayerId_;
}

std::string GameState::currentPlayerName() const
{
    return playerNames_->at(currentPlayer());
}

std::string GameState::getPlayerName(int playerId)
{
    return  playerNames_->at(playerId);
}

QString GameState::currentGamePhaseName() const
{
    if (currentPhase_ == 1)
    {
        return QString("movement");
    }
    else if (currentPhase_ == 2)
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
    GameState::currentPhase_ = nextPhase;
}

void GameState::changePlayerTurn(int nextPlayer)
{
    GameState::currentPlayerId_ = nextPlayer;
}

unsigned int GameState::getActionsLeft() const
{
    unsigned int actions = 0;
    for (auto it : playersPtr_)
    {
        if (it->getPlayerId() == currentPlayer())
        {
            actions = it->getActionsLeft();
            break;
        }
    }

    return actions;
}

void GameState::setActionsLeft(unsigned int amount)
{
    for (auto it : playersPtr_)
    {
        if (it->getPlayerId() == currentPlayer())
        {
            it->setActionsLeft(amount);
            break;
        }
    }
}
}

