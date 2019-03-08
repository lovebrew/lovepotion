#include "common/runtime.h"

void LuaSocket::Initialize()
{
    socketInitializeDefault();
}

void LuaSocket::Exit()
{
    socketExit();
}