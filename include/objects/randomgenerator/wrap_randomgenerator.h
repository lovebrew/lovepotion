#pragma once

int randomgeneratorNew(lua_State * L);

int randomgeneratorToString(lua_State * L);

int randomgeneratorGetSeed(lua_State * L);

int randomgeneratorGetState(lua_State * L);

int randomgeneratorRandom(lua_State * L);

int randomgeneratorRandomNormal(lua_State * L);

int randomgeneratorSetSeed(lua_State * L);

int randomgeneratorSetState(lua_State * L);

int initRandomGeneratorClass(lua_State *L);
