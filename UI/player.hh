#ifndef PLAYER_HH
#define PLAYER_HH

#include "iplayer.hh"


class Player : public Common::IPlayer
{
public:
    Player();
    Player(int playerId);
    ~Player();

    /**
     * @brief getPlayer gets the player id.
     * @return The identifier of the player.
     * @post Exception quarantee: nothrow
     */
    virtual int getPlayerId() const;

    /**
     * @brief setActionsLeft sets the player's remaining number of actions.
     * @param actionsLeft The number of actions left.
     * @post Actions left set to parameter value. Exception quarantee: nothrow
     */
    virtual void setActionsLeft(unsigned int actionsLeft);

    /**
     * @brief getActionsLeft returns the amount of actions left for the player
     * @return amount of actions
     * @post Exception quarantee: nothrow
     */
    virtual unsigned int getActionsLeft() const;

private:
    int _playerId;
    unsigned int _actionsLeft;
};

#endif // PLAYER_HH
