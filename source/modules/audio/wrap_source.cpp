#include "common/runtime.h"
#include "wrap_source.h"
#include "source.h"

#define CLASS_TYPE  LUAOBJ_TYPE_SOURCE
#define CLASS_NAME  "Source"

using love::Source;

int sourceNew(lua_State * L)
{
	const char * path = luaL_checkstring(L, 1);

	const char * type = luaL_optstring(L, 2, "static");

	Source * self = (Source *)luaobj_newudata(L, sizeof(* self));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	char * error = self->Init(path, type);

	if (error)
		luaL_error(L, error);

	return 1;
}

int sourcePlay(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Play();

	return 0;
}

int sourceGC(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	ndspChnWaveBufClear(self->audiochannel);

	linearFree(self->data);

	channelList[self->audiochannel] = false;
	
	return 0;
}

int initSourceClass(lua_State *L) 
{

	luaL_Reg reg[] = {
		{"new",			sourceNew			},
		{"play",		sourcePlay			},
		{"__gc",		sourceGC			},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, sourceNew, reg);

	return 1;

}