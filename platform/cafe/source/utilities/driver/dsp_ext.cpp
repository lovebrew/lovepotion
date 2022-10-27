#include <common/exception.hpp>

#include <utilities/driver/dsp_ext.hpp>
#include <utilities/log/logfile.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

using namespace love;

DSP<Console::CAFE>::DSP()
{}

void DSP<Console::CAFE>::Initialize()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    this->initialized = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096) == 0;

    if (!this->initialized)
        throw love::Exception("Failed to initialize DSP driver: (%s)!", Mix_GetError());
}

DSP<Console::CAFE>::~DSP()
{
    if (!this->initialized)
        return;

    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void DSP<Console::CAFE>::SetMasterVolume(float volume)
{
    AXSetMasterVolume(volume * 0x8000);
}

float DSP<Console::CAFE>::GetMasterVolume() const
{
    int16_t volume = AXGetMasterVolume();

    return volume / (float)0x8000;
}

bool DSP<Console::CAFE>::ChannelReset(size_t id)
{
    this->ChannelSetVolume(id, this->ChannelGetVolume(id));

    return true;
}

void DSP<Console::CAFE>::ChannelSetVolume(size_t id, float volume)
{
    /* sets between 0-128 */
    Mix_Volume(id, (volume * 128.0f));
}

float DSP<Console::CAFE>::ChannelGetVolume(size_t id)
{
    /* query volume for channel */
    return Mix_Volume(id, -1) / 128.0f;
}

size_t DSP<Console::CAFE>::ChannelGetSampleOffset(size_t id)
{
    return 0;
}

bool DSP<Console::CAFE>::ChannelAddBuffer(size_t id, Mix_Chunk* buffer, bool looping)
{
    int loops = (looping) ? -1 : 0;
    if (Mix_PlayChannel(id, buffer, loops) < 0)
        return false;

    return true;
}

void DSP<Console::CAFE>::ChannelPause(size_t id, bool paused)
{
    if (paused)
        Mix_Pause(id);
    else
        Mix_Resume(id);
}

bool DSP<Console::CAFE>::IsChannelPaused(size_t id)
{
    return Mix_Paused(id);
}

bool DSP<Console::CAFE>::IsChannelPlaying(size_t id)
{
    return Mix_Playing(id);
}

void DSP<Console::CAFE>::ChannelStop(size_t id)
{
    Mix_HaltChannel(id);
}
