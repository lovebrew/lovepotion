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

	const char * error = self->Init();

	if (error)
		console->ThrowError(error);

	return 1;
}

int udpSetSocketName(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	std::string ip = luaL_checkstring(L, 2);
	int port = luaL_optinteger(L, 3, 25545);

	int status = self->SetSocketName(ip.c_str(), port);

	lua_pushnumber(L, status);

	return 1;
}

int udpSetPeerName(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	std::string ip = luaL_checkstring(L, 2);
	int port = luaL_optinteger(L, 3, 25545);

	int status = self->SetPeerName(ip.c_str(), port);

	lua_pushnumber(L, status);

	return 1;
}

int udpSend(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	size_t len = 0;
	const char * data = luaL_checklstring(L, 2, &len);

	int sent = self->Send(data, len);

	if (sent < 0)
		lua_pushnil(L);
	else
		lua_pushinteger(L, sent);

	return 1;
}

int udpSendTo(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	size_t len = 0;
	const char * data = luaL_checklstring(L, 2, &len);
	
	const char * destination = luaL_checkstring(L, 3);
	int port = luaL_checkinteger(L, 4);

	int sent = self->SendTo(data, len, destination, port);

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
	{
		lua_pushstring(L, received);
		free(received);
	}

	return 1;
}

int udpReceiveFrom(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	char * buffer = (char *)calloc(1, 8192);
	char * address = (char *)calloc(1, 0x41);
	int port;

	int length = self->ReceiveFrom(buffer, address, &port);

	if (length == 0)
	{
		lua_pushnil(L);
		free(buffer);
		return 1;
	}
	
	lua_pushstring(L, buffer);
	lua_pushstring(L, address);
	lua_pushinteger(L, port);
	
	free(buffer);

	return 3;
}

int udpSetOption(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * option = luaL_checkstring(L, 2);
	bool enable = lua_toboolean(L, 3);

	int status = self->SetOption(option, enable);

	lua_pushboolean(L, status == 0);

	return 1;
}

int udpSetTimeout(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	float timeout = luaL_checknumber(L, 2);

	self->SetTimeOut(timeout);

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
		{"sendto",		udpSendTo},
		{"receive",		udpReceive},
		{"receivefrom", udpReceiveFrom},
		{"settimeout",	udpSetTimeout},
		{"setoption",	udpSetOption},
		{"close",		udpClose},
		{"__gc",		udpClose},
		{0, 0},
	};

	luaobj_newclass(L, CLASS_NAME, NULL, udpNew, reg);

	return 1;
}
