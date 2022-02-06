#pragma once

#include <lua.h>

#include "HTTPSClient.h"

HTTPSClient::Reply request(const HTTPSClient::Request& req);

extern "C" int luaopen_https(lua_State* L);
