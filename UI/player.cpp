#include<player.hh>

Student::Player::Player(int id)
{
    this->id_ = id;
}

Student::Player::~Player()
{
   return;
}

int Student::Player::getPlayerId() const
{
    return Student::Player::id_;
}


unsigned int Student::Player::getActionsLeft() const
{
    return Student::Player::actionsLeft_;
}

void Student::Player::setActionsLeft(unsigned int actionsLeft)
{
    Student::Player::actionsLeft_ = actionsLeft;
}


