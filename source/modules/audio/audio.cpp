#include "common/runtime.h"
#include "audio.h"
#include "wrap_source.h"
#include "source.h"

using love::Audio;

bool AUDIO_ENABLED = false;

int Audio::Stop(lua_State * L) 
{
	for (int i = 0; i <= 23; i++)
		ndspChnWaveBufClear(i);

	return 0;
}

int Audio::Play(lua_State * L)
{
	love::Source * source = (love::Source *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_SOURCE);

	source->Play();

	return 0;
}

int Audio::SetVolume(lua_State * L)
{
	float volume = luaL_checknumber(L, 1);
	volume = std::min(std::max(volume, 0.0f), 1.0f);

	ndspSetMasterVol(volume);

	return 0;
}

void audioExit()
{
	if (AUDIO_ENABLED) 
		ndspExit();
}

int audioInit(lua_State * L)
{
	AUDIO_ENABLED = !ndspInit();

	luaL_Reg reg[] = 
	{
		{ "newSource",	sourceNew				},
		{ "play",		love::Audio::Play		},
		{ "stop",		love::Audio::Stop		},
		{ "setVolume",	love::Audio::SetVolume	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}