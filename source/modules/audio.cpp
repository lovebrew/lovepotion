#include "common/runtime.h"
#include "modules/audio.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "objects/source/source.h"
#include "objects/source/wrap_source.h"

bool AUDIO_ENABLED = false;

void Audio::Initialize()
{
	AUDIO_ENABLED = !audoutInitialize();
	if (!AUDIO_ENABLED)
		printf("Audio output is disabled.");
	else
		audoutStartAudioOut();
}

int Audio::Play(lua_State * L)
{
	Source * self = (Source *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_SOURCE);

	self->Play();

	return 0;
}

int Audio::Stop(lua_State * L) 
{
	return 0;
}

int Audio::SetVolume(lua_State * L)
{
	return 0;
}

void Audio::Exit()
{
	if (AUDIO_ENABLED)
	{
		audoutStopAudioOut();
		audoutExit();
	}
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