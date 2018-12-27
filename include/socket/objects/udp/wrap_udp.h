#pragma once

int udpNew(lua_State * L);

int udpSend(lua_State * L);

int udpSendTo(lua_State * L);

int udpGetPeerName(lua_State * L);

int udpSetPeerName(lua_State * L);

int udpGetSockName(lua_State * L);

int udpSetSockName(lua_State * L);

int udpGC(lua_State * L);

int udpToString(lua_State * L);

int udpSetTimeout(lua_State * L);

int initUDPClass(lua_State * L);