#pragma once

int channelNew(lua_State * L);

int channelNew(lua_State * L, Channel * existing);

int channelPush(lua_State * L);

int channelPop(lua_State * L);

int channelToString(lua_State * L);

int initChannelClass(lua_State * L);