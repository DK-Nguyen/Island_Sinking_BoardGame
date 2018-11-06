#include<gamestate.hh>

GameState::~GameState()
{
    return;
}
GameState::GameState(Common::GamePhase game_phase, int first_player): _gamephase(game_phase), _current_player(first_player) {}

Common::GamePhase GameState::currentGamePhase() const
{
    return GameState::_gamephase;
}

int GameState::currentPlayer() const
{
    return GameState::_current_player;
}


void GameState::changeGamePhase(Common::GamePhase nextPhase)
{
    GameState::_gamephase = nextPhase;
}

void GameState::changePlayerTurn(int nextPlayer)
{
    GameState::_current_player = nextPlayer;
}


