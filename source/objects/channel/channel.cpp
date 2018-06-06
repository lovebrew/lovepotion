#include "common/runtime.h"

#include "objects/channel/channel.h"

#include <stdarg.h>

Channel::Channel(const string & name)
{
    this->name = name;
}

void Channel::Push(Variant variant)
{
    this->content.push(variant);
}

void Channel::Pop(lua_State * L)
{
    //default is nil
    Variant content;

    if (!this->content.empty())
        content = this->content.front();
    
    content.ToLua(L);

    if (!this->content.empty())
        this->content.pop();
}