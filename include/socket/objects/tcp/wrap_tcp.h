#pragma once

int tcpNew(lua_State * L);

int tcpOnAccept(lua_State * L, int sockfd);

int tcpGetPeerName(lua_State * L);

int tcpGetSockName(lua_State * L);

int tcpBind(lua_State * L);

int tcpAccept(lua_State * L);

int tcpReceive(lua_State * L);

int tcpSetTimeout(lua_State * L);

int tcpGC(lua_State  * L);

int tcpToString(lua_State * L);

int initTCPClass(lua_State * L);