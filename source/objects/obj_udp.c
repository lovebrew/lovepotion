// This code is licensed under the MIT Open Source License.

// Copyright (c) 2015 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <shared.h>

#define CLASS_TYPE  LUAOBJ_TYPE_SOCKET
#define CLASS_NAME  "Socket"

const char * socketInitUDP(lua_socket * self) {
	self->socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (self->socket < 0) {
		return "Failed to create a UDP socket.";
	}

	memset(&self->address, 0, sizeof(self->address));
	
	self->address.sin_family = AF_INET;
	
	int flags, blockSuccess;

	flags = fcntl(self->socket, F_GETFL, 0);
	if (flags < 0) {
		return "Failed to get flags for socket.";
	}

	blockSuccess = fcntl(self->socket, F_SETFL, flags + O_NONBLOCK);
	if (blockSuccess != 0) {
		return "Failed to set socket to non-blocking.";
	}

	//Set buffer things apparently

	//Receive Buffer
	int socketSize = SOCKETSIZE;
	setsockopt(self->socket, SOL_SOCKET, SO_RCVBUF, &socketSize, sizeof(socketSize));

  	//Send Buffer
  	setsockopt(self->socket, SOL_SOCKET, SO_SNDBUF, &socketSize, sizeof(socketSize));

	return NULL;
}

int newUDP(lua_State * L) { //require("socket").udp()
	lua_socket * self = luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	char * error = socketInitUDP(self);
	if (error) {
		luaError(L, error);
	}

	return 1;
}

int socketNewTCP(lua_State * L) {
	lua_socket * self = luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	socketInitTCP(self);

	return 1;
}

int socketInitTCP(lua_socket * self) {
	self->socket = socket(AF_INET, SOCK_STREAM, 0);

	if (self->socket < 0) {
		lua_pushnil(L);

		lua_pushstring(L, "Failed to create a TCP socket.");

		return 2;
	}

	self->address.sin_family = AF_INET;
	fcntl(self->socket, F_SETFL, fcntl(self->socket, F_GETFL, 0) | O_NONBLOCK);

	return 1;
}

int socketSetTimeout(lua_State * L) { //socket:settimeout
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	int setBlocking = luaL_checkinteger(L, 2);

	int flags = O_NONBLOCK;
	if (setBlocking != 0) {
		flags = ~O_NONBLOCK;
	}

	fcntl(self->socket, F_SETFL, flags);
}

int socketClose(lua_State *L) { //socket:close
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	closesocket(self->socket);

	return 0;
}

int socketGetPeerName(lua_State * L) { //socket:getpeername
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	struct sockaddr_in address;

	socklen_t addressSize = sizeof(address);

	getpeername(self->socket, (struct sockaddr *)&address, &addressSize);

	lua_pushstring(L, inet_ntoa(address.sin_addr));

	lua_pushnumber(L, ntohs(address.sin_port));

	return 2;
}

int socketGetSocketName(lua_State * L) { //socket:getsockname
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	struct sockaddr_in address;
	socklen_t addressSize = sizeof(address);

	getsockname(self->socket, (struct sockaddr *)&address, &addressSize);

	lua_pushstring(L, inet_ntoa(address.sin_addr));

	lua_pushnumber(L, ntohs(address.sin_port));
}

int socketReceive(lua_State * L) { //socket:receive -- get data from connected server (clients)
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);
	int datagramSize = 0;
	
	if (lua_isnumber(L, 2)) {
		datagramSize = luaL_checkinteger(L, 2);
	} else if (lua_isnoneornil(L, 2)) { 
		datagramSize = SOCKETSIZE;
	}
	
	char * buffer = malloc(datagramSize + 1);
	
	int length = recv(self->socket, buffer, datagramSize, 0);
	
	*(buffer + length) = 0x0;
	
	lua_pushstring(L, buffer);
	
	free(buffer);
		
	return 1;
}

int socketReceiveFrom(lua_State * L) { //socket:receivefrom -- get data from connected server (servers)
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	int datagramSize = 0;
	
	if (lua_isnumber(L, 2)) {
		datagramSize = luaL_checkinteger(L, 2);
	} else if (lua_isnoneornil(L, 2)) { 
		datagramSize = SOCKETSIZE;
	}

	struct sockaddr_in fromAddress = {0};
	
	struct hostent * hostInfo = gethostbyname(self->ip);
	if (!hostInfo) {
		lua_pushnil(L);
		
		return 1;
	}
	
	fromAddress.sin_addr = *(struct in_addr *)hostInfo->h_addr;
	fromAddress.sin_port = htons(self->port);
	fromAddress.sin_family = AF_INET;
	
	char * buffer = malloc(datagramSize + 1);
	int size = recvfrom(self->socket, buffer, datagramSize, 0, (struct sockaddr *)&fromAddress, NULL);
	*(buffer + size) = 0x0;
	
	lua_pushstring(L, buffer);
	lua_pushstring(L, inet_ntoa(fromAddress.sin_addr));
	lua_pushnumber(L, ntohs(fromAddress.sin_port));

	return 3;
}

int socketSetOption(lua_State * L)
{
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	const char * option = luaL_checkstring(L, 2);
	
	bool enabled = lua_isboolean(L, 3); 

	if (strncmp(option, "broadcast", 9) == 0)
	{
		setsockopt(self->socket, SOL_SOCKET, 0x0020, (char *)&enabled, sizeof(bool));
	}
	else if (strncmp(option, "dontroute", 9) == 0)
	{
		setsockopt(self->socket, SOL_SOCKET, 0x0010, (char *)&enabled, sizeof(bool));
	}
}

int socketSend(lua_State * L) {
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	size_t stringLength;
	char * data = luaL_checklstring(L, 2, &stringLength);
	
	struct hostent * hostInfo = gethostbyname(self->ip);

	if (!hostInfo) {
		lua_pushnil(L);

		return 1;
	}

	struct sockaddr_in addressTo = {0};

	addressTo.sin_addr = *(struct in_addr *)hostInfo->h_addr;
	addressTo.sin_port = htons(self->port);
	addressTo.sin_family = AF_INET;

	size_t sent = sendto(self->socket, data, stringLength, 0, (struct sockaddr *)&addressTo, sizeof(addressTo));

	lua_pushnumber(L, sent);

	return 1;
}

int socketSendTo(lua_State * L) {
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);
	
	size_t stringLength;
	char * data = luaL_checklstring(L, 2, &stringLength);

	char * hostName = luaL_checkstring(L, 3);
	
	int port = luaL_checkinteger(L, 4);
	
	struct hostent * hostInfo = gethostbyname(hostName);

	if (!hostInfo) {
		lua_pushnil(L);

		return 1;
	}

	struct sockaddr_in addressTo = {0};

	addressTo.sin_addr = *(struct in_addr *)hostInfo->h_addr;
	addressTo.sin_port = htons(port);
	addressTo.sin_family = AF_INET;

	size_t sent = sendto(self->socket, data, stringLength, 0, (struct sockaddr *)&addressTo, sizeof(addressTo));

	lua_pushnumber(L, sent);

	return 1;
}

int socketListen(lua_State * L) {
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	int maxListeners = luaL_optnumber(L, 2, 16);

	listen(self->socket, maxListeners);

	return 0;
}

int socketConnect(lua_State * L) {
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	char * address = luaL_checkstring(L, 2);

	int port = luaL_checkinteger(L, 3);

	self->host = gethostbyname(address);

	if (!self->host) {
		lua_pushnil(L);

		lua_pushstring(L, "Unable to resolve host.");
	}

	self->address.sin_port = htons(port);

	bcopy(self->host->h_addr, self->address.sin_addr.s_addr, self->host->h_length);

	int connectionSuccess = connect(self->socket, (const struct sockaddr *)&self->address, sizeof(self->address));

	if (connectionSuccess < 0) {
		lua_pushnil(L);

		lua_pushstring(L, "Could not connect to host.");

		return 2;
	}

	lua_pushboolean(L, 1);

	return 1;
}

int socketSetPeerName(lua_State * L) {
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	char * address = luaL_checkstring(L, 2);

	int port = luaL_checkinteger(L, 3);

	self->host = gethostbyname(address);

	if (self->host == NULL) {
		luaError(L, "Could not resolve host name");

		return 0;
	}

	self->address.sin_port = htons(port);

	bcopy((char *)self->host->h_addr, (char *)&self->address.sin_addr.s_addr, self->host->h_length); //Copy host info to the new address
	
	self->ip = address; //Copy host info to our IP info
	
	self->port = port;
	
	if (connect(self->socket, (const struct sockaddr *)&self->address, sizeof(self->address)) < 0) {
		luaError(L, "Could not connect to host.");

		return 0;
	}

	lua_pushboolean(L, 1);

	return 1;
}

int socketSetSockName(lua_State * L) {
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	self->address.sin_addr.s_addr = luaL_checkstring(L, 2);

	self->address.sin_port = luaL_checkinteger(L, 3);

	int connectionSuccess = bind(self->socket, (const struct sockaddr *)&self->address, sizeof(self->address));

	if (connectionSuccess < 0) {
		lua_pushnil(L);

		lua_pushstring(L, "Could not connect to host.");

		return 2;
	}

	lua_pushnumber(L, 1);

	return 1;
}

int socketAccept(lua_State * L) {
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_socket * tcp = socketNewTCP(L);

	socklen_t addressSize = sizeof(tcp->address);

	tcp->socket = accept(self->socket, (struct sockaddr *)&tcp->address, &addressSize);

	if (tcp->socket < 0) {
		lua_pushnil(L);

		return 1;
	}

	return 1;
}

int socketBind(lua_State * L) {
	lua_socket * self = luaobj_checkudata(L, 1, CLASS_TYPE);

	char * address = luaL_checkstring(L, 2);

	int port = luaL_checkinteger(L, 3);

	self->address.sin_addr.s_addr = address;
	self->address.sin_port = htons(port);

	bind(self->socket, (struct sockaddr *)&self->address, sizeof(self->address));

	return 0;
}

int initSocketClass(lua_State *L) {

	luaL_Reg reg[] = {
		{"new", newUDP},
		{"close",	socketClose},
		{"getpeername",	socketGetPeerName},
		{"setpeername", socketSetPeerName},
		{"getsockname", socketGetSocketName},
		{"setsockname", socketSetSockName},
		{"settimeout", socketSetTimeout},
		{"receive",  socketReceive},
		{"receivefrom", socketReceiveFrom},
		{"send", socketSend},
		{"sendto", socketSendTo},
		{"bind", socketBind},
		{"accept", socketAccept},
		{"connect", socketConnect},
		{"listen", socketListen},
		{"setoption", socketSetOption},
		{0,	0},
	};

  luaobj_newclass(L, CLASS_NAME, NULL, newUDP, reg);

  return 1;

}
