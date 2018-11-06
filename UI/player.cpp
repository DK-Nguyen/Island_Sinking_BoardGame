#include<player.hh>

Player::~Player()
{
   return;
}
Player::Player(int playerId): _playerId(playerId){}

int Player::getPlayerId() const
{
    return Player::_playerId;
}

unsigned int Player::getActionsLeft() const
{
    return Player::_actionsLeft;
}

void Player::setActionsLeft(unsigned int actionsLeft)
{
    Player::_actionsLeft = actionsLeft;
}


