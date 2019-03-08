#include "common/runtime.h"

u32 * SOCKET_BUFFER;
Result SOCKETS_INIT;

void LuaSocket::Initialize()
{
    SOCKET_BUFFER = (u32 *)memalign(0x1000, 0x100000);

    SOCKETS_INIT = socInit(SOCKET_BUFFER, 0x100000);
}

void LuaSocket::Exit()
{
    if (SOCKETS_INIT == 0)
        socExit();
}