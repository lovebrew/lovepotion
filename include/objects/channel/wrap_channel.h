#pragma once

int channelNew(lua_State * L);

int channelPush(lua_State * L);

int channelPop(lua_State * L);

int channelToString(lua_State * L);

int initChannelClass(lua_State * L);