#ifndef PLAYER_HH
#define PLAYER_HH

#include "iplayer.hh"
#include <string>
namespace  Student{
class Player : public Common::IPlayer
{
public:
    Player();
    Player(int id_);
    ~Player();

    virtual int getPlayerId() const;

    virtual void setActionsLeft(unsigned int actionsLeft);

    virtual unsigned int getActionsLeft() const;

private:
    int id_;
    unsigned int actionsLeft_;
};
}

#endif // PLAYER_HH
