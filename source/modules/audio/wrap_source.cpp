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

	const char * error = self->Init(path, type);

	if (error)
		console->ThrowError(error);

	return 1;
}

int sourcePlay(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Play();

	return 0;
}

int sourceSetLooping(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);
	
	bool shouldLoop = lua_toboolean(L, 2);
	self->SetLooping(shouldLoop);

	return 0;
}

int sourceIsPlaying(lua_State * L)
{
	Source * self =  (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushboolean(L, self->IsPlaying());

	return 1;
}

int sourceStop(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Stop();

	return 0;
}

int sourceGetDuration(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->GetDuration());

	return 1;
}

int sourceSetVolume(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	float volume = luaL_checknumber(L, 2);

	self->SetVolume(volume);

	return 0;
}

int sourceTell(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);
	
	lua_pushnumber(L, self->Tell());

	return 1;
}

void sourceStream(void * arg)
{
	while (updateAudioThread)
	{
		for (auto &it : streams)
		{
			if (it != nullptr)
				it->Update();
		}
		
		svcSleepThread(1000000ULL);
	}
}

int sourceGC(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Collect();
	
	if (self->IsStatic())
		return 0;

	streams.erase(std::remove(streams.begin(), streams.end(), self), streams.end());

	return 0;
}

int initSourceClass(lua_State *L) 
{

	luaL_Reg reg[] = {
		{ "new",				sourceNew			},
		{ "play",				sourcePlay			},
		{ "setLooping",			sourceSetLooping	},
		{ "stop",				sourceStop			},
		{ "getDuration",		sourceGetDuration	},
		{ "tell",				sourceTell			},
		{ "isPlaying",			sourceIsPlaying		},
		{ "setVolume",			sourceSetVolume		},
		{ "__gc",				sourceGC			},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, sourceNew, reg);

	return 1;

}