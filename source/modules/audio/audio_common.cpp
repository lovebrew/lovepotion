#include "common/runtime.h"
#include "modules/audio/audio.h"
#include "modules/audio/pool/pool.h"

using namespace love;

std::array<bool, 24> Audio::channels;

int Audio::GetOpenChannel()
{
    for (size_t i = 0; i < Audio::channels.size(); i++)
    {
        if (!Audio::channels[i])
            return i;
    }

    return -1;
}

void Audio::FreeChannel(size_t channel)
{
    if (Audio::channels[channel])
        Audio::channels[channel] = false;
}

float Audio::GetVolume() const
{
    return this->volume;
}

Source * Audio::NewSource(Decoder * decoder)
{
    return new Source(this->pool, decoder);
}

Source * Audio::NewSource(SoundData * sound)
{
    return new Source(this->pool, sound);
}

bool Audio::Play(Source * source)
{
    return source->Play();
}

void Audio::Stop(Source * source)
{
    source->Stop();
}

void Audio::Pause(Source * source)
{
    source->Pause();
}

void Audio::Pause()
{}

void Audio::Stop()
{

}
