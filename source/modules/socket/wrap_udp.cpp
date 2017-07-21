#include "common/runtime.h"
#include "wrap_udp.h"
#include "udp.h"

#define CLASS_NAME "UDP"
#define CLASS_TYPE LUAOBJ_TYPE_UDP

using love::UDP;

int udpNew(lua_State * L)
{
	UDP * self = (UDP *)luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	char * error = self->Init();

	if (error)
		console->ThrowError(error);

	return 1;
}

int udpSetSocketName(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * ip = luaL_checkstring(L, 2);
	int port = luaL_optinteger(L, 3, 25545);

	int status = self->SetSocketName(ip, port);

	lua_pushnumber(L, status);

	return 1;
}

int udpSetPeerName(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * ip = luaL_checkstring(L, 2);
	int port = luaL_optinteger(L, 3, 25545);

	int status = self->SetPeerName(ip, port);

	lua_pushnumber(L, status);

	return 1;
}

int udpSend(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	size_t len;
	const char * data = luaL_checklstring(L, 2, &len);

	int sent = self->Send(data, len);

	if (sent < 0)
		lua_pushnil(L);
	else
		lua_pushinteger(L, sent);

	return 1;
}

int udpReceive(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	char * received = self->Receive();

	if (received == nullptr)
		lua_pushnil(L);
	else
		lua_pushstring(L, received);

	return 1;
}

int udpSetTimeout(lua_State * L)
{
	return 0;
}

int udpClose(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Close();

	return 0;
}

int initUDPClass(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{"new",			udpNew},
		{"setsockname", udpSetSocketName},
		{"setpeername", udpSetPeerName},
		{"send",		udpSend},
		{"receive",		udpReceive},
		{"settimeout",	udpSetTimeout},
		{"close",		udpClose},
		{"__gc",		udpClose},
		{0, 0},
	};

	luaobj_newclass(L, CLASS_NAME, NULL, udpNew, reg);

	return 1;
}