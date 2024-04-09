#pragma once

struct lua_State;

extern "C"
{
    extern int luaopen_https(lua_State*);
}

static constexpr int STDIO_PORT = 8000;

const char* love_getVersion();

const char* love_getCodename();

int love_initialize(lua_State* L);

int love_isVersionCompatible(lua_State* L);

int love_openConsole(lua_State* L);

int love_print(lua_State* L);

int love_openNoGame(lua_State* L);

int love_openArg(lua_State* L);

int love_openCallbacks(lua_State* L);

int love_openBoot(lua_State* L);
