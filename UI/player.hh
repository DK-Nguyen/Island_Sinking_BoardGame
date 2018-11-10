#ifndef PLAYER_HH
#define PLAYER_HH

#include "iplayer.hh"
#include <string>

class Player : public Common::IPlayer
{
public:
    Player();
    Player(int id);
    ~Player();

    virtual int getPlayerId() const;

    virtual void setActionsLeft(unsigned int actionsLeft);

    virtual unsigned int getActionsLeft() const;

private:
    int id;
    unsigned int actions_left;
};

#endif // PLAYER_HH
