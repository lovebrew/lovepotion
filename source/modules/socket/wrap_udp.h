#pragma once

int udpNew(lua_State * L);

int udpSetSocketName(lua_State * L);

int udpSetPeerName(lua_State * L);

int udpSend(lua_State * L);

int udpSendTo(lua_State * L);

int udpReceive(lua_State * L);

int udpReceiveFrom(lua_State * L);

int udpSetOption(lua_State * L);

int udpSetTimeout(lua_State * L);

int udpClose(lua_State * L);

int initUDPClass(lua_State * L);