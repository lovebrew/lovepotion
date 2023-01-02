#include <common/exception.hpp>

#include <utilities/driver/dsp_ext.hpp>

#include <sndcore2/voice.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

using namespace love;

static std::array<int, 24> channelOffsets;

static void SDL_ChnEffectEvent(int channel, void* stream, int length, void* userdata)
{
    channelOffsets[channel] += length;
}

DSP<Console::CAFE>::DSP()
{}

void DSP<Console::CAFE>::Initialize()
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    this->initialized = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096) == 0;

    if (!this->initialized)
        throw love::Exception("Failed to initialize DSP driver: (%s)!", Mix_GetError());

    Mix_AllocateChannels(24);
    OSInitEvent(&this->event, 1, OS_EVENT_MODE_AUTO);
}

DSP<Console::CAFE>::~DSP()
{
    for (size_t channel = 0; channel < 24; channel++)
        Mix_UnregisterAllEffects(channel);

    OSResetEvent(&this->event);

    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void DSP<Console::CAFE>::Update()
{}

void DSP<Console::CAFE>::SetMasterVolume(float volume)
{
    Mix_Volume(-1, (volume * 128));
}

float DSP<Console::CAFE>::GetMasterVolume() const
{
    return Mix_Volume(-1, -1) / 128.0f;
}

bool DSP<Console::CAFE>::ChannelReset(size_t id)
{
    this->ChannelSetVolume(id, this->ChannelGetVolume(id));
    channelOffsets[id] = 0;

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

size_t DSP<Console::CAFE>::ChannelGetSampleOffset(size_t id, int bitDepth)
{
    return channelOffsets[id] / bitDepth * 8;
}

bool DSP<Console::CAFE>::ChannelAddBuffer(size_t id, Mix_Chunk* buffer, bool looping)
{
    int loops = (looping) ? -1 : 0;

    Mix_RegisterEffect(id, SDL_ChnEffectEvent, nullptr, nullptr);
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
