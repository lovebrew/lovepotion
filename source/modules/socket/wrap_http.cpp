#include "common/runtime.h"
#include "http.h"
#include "wrap_http.h"

using love::HTTP;

int httpRequest(lua_State * L)
{
	const char * url = luaL_checkstring(L, 1);
	const char * method = luaL_optstring(L, 2, "GET");

	HTTP * http = new HTTP(url);
	
	HTTPC_RequestMethod requestMethod = HTTPC_METHOD_GET;
	if (strncmp(method, "POST", 4) == 0)
		requestMethod = HTTPC_METHOD_POST;

	do
	{
		http->OpenContext(requestMethod, true); //disable SSL?
		http->AddRequestHeaderField(std::string("User-Agent"), std::string("LovePotion/") + std::string(love::VERSION));
		http->AddRequestHeaderField("Connection", "Keep-Alive");
		http->BeginRequest();
	} while (http->Redirected());

	http->Download();

	char * buffer = (char *)http->GetBuffer();

	lua_pushlstring(L, buffer, http->GetSize());
	lua_pushnumber(L, http->GetStatusCode());

	http->Close();

	return 2;
}

int initHTTP(lua_State * L)
{
	httpcInit(0);

	luaL_Reg reg[] = 
	{
		{"request", httpRequest},
		{0, 0},
	};

	luaL_newlib(L, reg);

	return 1;
}