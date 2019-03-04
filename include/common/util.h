#pragma once

#define luaL_dobuffer(L, b, n, s) \
    (luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define LOVE_TIMER_STEP "if love.timer then love.timer.step() end"

#define LOVE_UPDATE     "if love.update then love.update(love.timer.getDelta()) end"

#define LOVE_DRAW       "if love.draw then love.draw() end"

#define LOG(fmt, ...) fprintf(logFile, "%s:%d:\n" fmt "\n", __PRETTY_FUNCTION__, __LINE__, ## __VA_ARGS__)

#include <exception>

void love_getfield(lua_State * L, const char * field);

int love_preload(lua_State * L, lua_CFunction function, const char * name);

bool love_is_registered(lua_State * L, void * object);

void stack_dump(lua_State * L);

void love_register(lua_State * L, int index, void * object);

void love_push_userdata(lua_State * L, void * object);

int love_get_registry(lua_State * L, REGISTRY registry);

int luax_tracevack(lua_State * L);

double clamp(double low, double x, double high);

u32 NextPO2(u32 in);