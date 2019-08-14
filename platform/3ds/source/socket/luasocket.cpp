#include "common/runtime.h"

void LuaSocket::Initialize()
{
    SOCKET_BUFFER = (u32 *)memalign(0x1000, 0x100000);
    INITIALIZED = socInit(SOCKET_BUFFER, 0x100000);
}

void LuaSocket::Exit()
{
    if (INITIALIZED == 0)
        socExit();
}
