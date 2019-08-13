#pragma once

class LuaSocket
{
    public:
        static void Initialize();

        static int InitSocket(lua_State * L);

        static int InitHTTP(lua_State * L);

        static void Exit();

    private:
        static inline u32 * SOCKET_BUFFER = 0;

        static inline Result INITIALIZED = 0;
};
