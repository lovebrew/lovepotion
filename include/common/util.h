#pragma once

#define luaL_dobuffer(L, b, n, s) \
    (luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define LOVE_TIMER_STEP "if love.timer then love.timer.step() end"

#define LOVE_UPDATE     "if love.update then love.update(love.timer.getDelta()) end"

#define LOVE_DRAW       "if love.draw then love.draw() end"

#define CONFIG_3D_SLIDERSTATE (*(volatile float *)0x1FF81080)

#include <exception>
#include "common/types.h"

void love_getfield(lua_State * L, const char * field);

int love_preload(lua_State * L, lua_CFunction function, const char * name);

bool love_is_registered(lua_State * L, void * object);

void love_register(lua_State * L, int index, void * object);

void love_push_userdata(lua_State * L, void * object);

int love_get_registry(lua_State * L, REGISTRY registry);

u32 NextPO2(u32 in);

double clamp(double low, double x, double high);

extern std::vector<std::string> KEYS;

extern std::vector<std::string> GAMEPAD_AXES;

extern std::map<int, std::string> LANGUAGES;

extern std::map<int, std::string> REGIONS;