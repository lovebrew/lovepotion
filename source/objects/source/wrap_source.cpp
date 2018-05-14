#include "runtime.h"

#include "source.h"
#include "wrap_source.h"

#define CLASS_TYPE LUAOBJ_TYPE_SOURCE
#define CLASS_NAME "Source"

std::map<int, Source *> streams;

int sourceNew(lua_State * L)
{
	const char * path = luaL_checkstring(L, 1);

	const char * type = luaL_checkstring(L, 2);

	bool stream = (strncmp(type, "stream", 6) == 0) ? true : false;

	void * raw_self = luaobj_newudata(L, sizeof(Source));

	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

	Source * self = new (raw_self) Source(path, stream);

	if (stream)
		streams[self->GetAudioChannel()] = self;

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
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushboolean(L, self->IsPlaying());

	return 1;
}

int sourceStop(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	self->Stop();

	return 0;
}

int sourceSetVolume(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	float volume = luaL_checknumber(L, 2);

	self->SetVolume(volume);

	return 0;
}

/*int sourceGetDuration(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

	lua_pushnumber(L, self->GetDuration());

	return 1;
}

int sourceTell(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);
	
	lua_pushnumber(L, self->Tell());

	return 1;
}*/

void sourceStream(void * arg)
{
	while (updateAudioThread)
	{
		for (uint i = 0; i <= 23; i++)
		{
			if (streams.find(i) != streams.end())
				streams[i]->Update();
		}

		svcSleepThread(1e6);
	}
}

int sourceGC(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);
	
	if (self->IsStatic())
	{
		self->~Source();

		return 0;
	}

	self->Stop();

	streams.erase(self->GetAudioChannel());
	
	self->~Source();

	return 0;
}

int initSourceClass(lua_State *L) 
{
	luaL_Reg reg[] = 
	{
		{ "new",				sourceNew			},
		{ "play",				sourcePlay			},
		{ "setLooping",			sourceSetLooping	},
		{ "stop",				sourceStop			},
		/*{ "getDuration",		sourceGetDuration	},
		{ "tell",				sourceTell			},*/
		{ "isPlaying",			sourceIsPlaying		},
		{ "setVolume",			sourceSetVolume		},
		{ "__gc",				sourceGC			},
		{ 0, 0 },
	};

	luaobj_newclass(L, CLASS_NAME, NULL, sourceNew, reg);

	return 1;
}