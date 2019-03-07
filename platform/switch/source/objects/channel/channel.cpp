#include "common/runtime.h"
#include "objects/channel/channel.h"

Channel::Channel(const string & name) : Object("Channel")
{
    this->name = name;
}

Channel::Channel() : Object("Channel")
{
    this->name = "";
}

void Channel::Push(const Variant & variant)
{
    this->content.push(variant);
}

bool Channel::Pop(Variant * variant)
{
    if (this->content.empty())
        return false;
    
    *variant = this->content.front();
    this->content.pop();

    return true;
}