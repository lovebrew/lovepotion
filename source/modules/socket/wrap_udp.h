#pragma once

int udpNew(lua_State * L);

int udpSetSocketName(lua_State * L);

int udpSetPeerName(lua_State * L);

int udpSend(lua_State * L);

int udpReceive(lua_State * L);

int udpSetTimeout(lua_State * L);

int udpClose(lua_State * L);

int initUDPClass(lua_State * L);