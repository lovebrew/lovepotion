#include "common/runtime.h"
#include "socket/common.h"

#include "socket/objects/udp/udp.h"
#include "socket/objects/udp/wrap_udp.h"

#define CLASS_NAME "UDP"
#define CLASS_TYPE LUAOBJ_TYPE_UDP

int udpNew(lua_State * L)
{
	void * raw_self = luaobj_newudata(L, sizeof(UDP));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	UDP * self = new (raw_self) UDP();

	return 1;
}

//UDP:send
int udpSend(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	size_t length = 0;
	const char * datagram = luaL_checklstring(L, 2, &length);

	int sent = self->Send(datagram, length);

	if (sent < 0)
		lua_pushnil(L);
	else
		lua_pushinteger(L, sent);

	return 1;
}

int udpSendTo(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	size_t length = 0;
	const char * datagram = luaL_checklstring(L, 2, &length);

	const char * ip = luaL_checkstring(L, 3);
	int port = luaL_checknumber(L, 4);

	int sent = self->SendTo(datagram, length, ip, port);

	if (sent < 0)
		lua_pushnil(L);
	else
		lua_pushinteger(L, sent);

	return 1;
}

//UDP:setpeername
int udpSetPeerName(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	string ip = luaL_checkstring(L, 2);
	int port = luaL_checknumber(L, 3);

	int status = self->SetPeerName(ip, port);

	lua_pushinteger(L, status);

	return 1;
}

//UDP:setsockname
int udpSetSockName(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	string ip = luaL_checkstring(L, 2);
	int port = luaL_checknumber(L, 3);

	int status = self->SetSockName(ip, port);

	lua_pushinteger(L, status);

	return 1;
}

//UDP:receivefrom
int udpReceiveFrom(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	char buffer[SOCKET_BUFFERSIZE+1];
	char address[0x41];	
	int port;

	int length = self->ReceiveFrom(buffer, address, &port);

	if (length == 0)
	{
		lua_pushnil(L);
		return 1;
	}
	
	lua_pushstring(L, buffer);
	lua_pushstring(L, address);
	lua_pushinteger(L, port);

	return 3;
}

//UDP:receive
int udpReceive(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	char buffer[SOCKET_BUFFERSIZE];
	int length = self->Receive(buffer);

	if (length == 0)
	{
		lua_pushnil(L);
		return 1;
	}

	lua_pushstring(L, buffer);

	return 1;
}

int udpClose(lua_State * L)
{
	UDP * self = (UDP *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Close();

	return 0;
}

int udpGC(lua_State * L)
{
	return 0;
}

int initUDPClass(lua_State * L) 
{
	luaL_Reg reg[] = 
	{
		{"new",						udpNew			},
		{"send",					udpSend			},
		{"setpeername",				udpSetPeerName	},
		{"setsockname",				udpSetSockName	},
		{"receivefrom",				udpReceiveFrom	},
		{"receive",					udpReceive		},
		{"sendto",					udpSendTo		},
		{"close",					udpClose		},
		{"__gc",					udpGC			},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, udpNew, reg);

	return 1;
}