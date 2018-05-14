#include "runtime.h"
#include "audio.h"

#include "source.h"
#include "wrap_source.h"

bool AUDIO_ENABLED = false;
vector<bool> audioChannels(24);

void Audio::Initialize()
{
	if (SDL_Init(SDL_INIT_AUDIO) != 0)
		Console::ThrowError("Couldn't open audio.");

	if(Mix_OpenAudio(44100, AUDIO_S16, 2, 4096) != 0)
		Console::ThrowError("Couldn't open audio.");

	Mix_AllocateChannels(23);

	for (uint i = 0; i < audioChannels.size(); i++)
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
	//for (int i = 0; i <= 23; i++)
	//	ndspChnWaveBufClear(i);

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
	for (uint i = 0; i <= audioChannels.size(); i++)
	{
		if (!audioChannels[i])
		{
			audioChannels[i] = true;
			return i;
		}
	}
	return -2;
}

void Audio::Exit()
{
	Mix_Quit();
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
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