#include "common/runtime.h"
#include "modules/audio.h"

#include "objects/source/source.h"
#include "objects/source/wrap_source.h"

vector<bool> audioChannels(8);

void Audio::Initialize()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    if (Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 4096) != 0)
        Love::RaiseError("Failed to load audio!\n");
}

int Audio::GetOpenChannel()
{
    for (uint i = 0; i < audioChannels.size(); i++)
    {
        if (!audioChannels[i])
        {
            audioChannels[i] = true;
            return i;
        }
    }
    return -1;
}

//Löve2D Functions

//love.audio.play
int Audio::Play(lua_State * L)
{
    Source * self = (Source *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_SOURCE);

    if (!self->IsPlaying())
        self->Play();

    return 0;
}

//love.audio.stop
int Audio::Stop(lua_State * L) 
{
    if (lua_isnoneornil(L, 1))
    {
        Mix_HaltChannel(-1);
        Mix_HaltMusic();
    }
    else
    {
        Source * self = (Source *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_SOURCE);

        self->Stop();
    }

    return 0;
}

//love.audio.pause
int Audio::Pause(lua_State * L)
{
    if (lua_isnoneornil(L, 1))
    {
        Mix_Pause(-1);
        Mix_PauseMusic();
    }
    else
    {
        Source * self = (Source *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_SOURCE);

        self->Pause();
    }

    return 0;
}

//love.audio.resume
int Audio::Resume(lua_State * L)
{
    if (lua_isnoneornil(L, 1))
    {
        Mix_Resume(-1);
        Mix_ResumeMusic();
    }
    else
    {
        Source * self = (Source *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_SOURCE);

        self->Resume();
    }

    return 0;
}

//love.audio.setVolume
int Audio::SetVolume(lua_State * L)
{
    double volume = clamp(0, luaL_checknumber(L, 1), 1);

    Mix_Volume(-1, MIX_MAX_VOLUME * volume);
    Mix_VolumeMusic(MIX_MAX_VOLUME * volume);

    return 0;
}

void Audio::Exit()
{
    Mix_CloseAudio();

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

//End Löve2D Functions

int Audio::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "newSource",  sourceNew },
        { "pause",      Pause     },
        { "play",       Play      },
        { "resume",     Resume    },
        { "setVolume",  SetVolume },
        { "stop",       Stop      },
        { 0, 0 },
    };

    luaL_newlib(L, reg);
    
    return 1;
}