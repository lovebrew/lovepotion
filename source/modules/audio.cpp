#include "runtime.h"
#include "audio.h"

#include "source.h"
#include "wrap_source.h"

bool AUDIO_ENABLED = false;
vector<bool> audioChannels(24);

void Audio::Initialize()
{
	AUDIO_ENABLED = !ndspInit();
	if (!AUDIO_ENABLED)
		printf("Audio output is disabled.\nPlease dump your dspfirm.");

	for (uint i = 0; i < 24; i++)
		audioChannels[i] = false;
}

int Audio::Play(lua_State * L)
{
	Source * source = (Source *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_SOURCE);

	source->Play();

	return 0;
}

int Audio::Stop(lua_State * L) 
{
	for (int i = 0; i <= 23; i++)
		ndspChnWaveBufClear(i);

	return 0;
}

int Audio::SetVolume(lua_State * L)
{
	float volume = luaL_checknumber(L, 1);
	volume = std::min(std::max(volume, 0.0f), 1.0f);

	ndspSetMasterVol(volume);

	return 0;
}

int Audio::GetOpenChannel()
{
	for (uint i = 0; i < 24; i++)
	{
		if (!audioChannels[i])
		{
			audioChannels[i] = true;
			return i;
		}
	}
	return -1;
}

void Audio::Exit()
{
	if (AUDIO_ENABLED) 
		ndspExit();
}

int Audio::Register(lua_State * L)
{
	luaL_Reg reg[] = 
	{
		{ "newSource",	sourceNew	},
		{ "play",		Play		},
		{ "stop",		Stop		},
		{ "setVolume",	SetVolume	},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);
	
	return 1;
}