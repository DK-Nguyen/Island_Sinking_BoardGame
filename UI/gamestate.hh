#ifndef GAMESTATE_HH
#define GAMESTATE_HH

#include "igamestate.hh"


class GameState : public Common::IGameState
{
public:
    GameState(Common::GamePhase game_phase, int first_player);
    ~GameState();
    /**
     * @brief currentGamePhase tells the phase of the game.
     * @return The phase of the game.
     * @post Exception quarantee: nothrow
     */
    virtual Common::GamePhase currentGamePhase() const;

    /**
     * @brief currentPlayer tells the player in turn.
     * @return The identifier of the player in turn.
     * @post Excetion quarantee: nothrow
     */
    virtual int currentPlayer() const;

    /**
     * @brief changeGamePhase sets the phase of the game.
     * @param newPhase The next phase of the game.
     * @post Game phase changed. Exception quarantee: basic
     */
    virtual void changeGamePhase(Common::GamePhase nextPhase);

    /**
     * @brief changePlayerTurn sets the player in turn.
     * @param nextPlayer The identifier of the next player in turn.
     * @post Turn changed to player nextPlayer. Exception quarantee: basic
     */
    virtual void changePlayerTurn(int nextPlayer);

private:
    Common::GamePhase _gamephase;
    int _current_player;
};

#endif // GAMESTATE_HH
