#include<player.hh>

Player::Player(int id)
{
    this->id = id;
}

Player::~Player()
{
   return;
}

int Player::getPlayerId() const
{
    return Player::id;
}

unsigned int Player::getActionsLeft() const
{
    return Player::actions_left;
}

void Player::setActionsLeft(unsigned int actionsLeft)
{
    Player::actions_left = actionsLeft;
}


