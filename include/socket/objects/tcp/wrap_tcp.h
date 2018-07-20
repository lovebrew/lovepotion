#pragma once

int tcpNew(lua_State * L);

int tcpBind(lua_State * L);

int tcpAccept(lua_State * L);

int tcpGC(lua_State  * L);

int tcpToString(lua_State * L);

int initTcpClass(lua_State * L);