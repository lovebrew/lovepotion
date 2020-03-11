#include "common/runtime.h"
#include "modules/audio/audio.h"

using namespace love;

Audio::Audio()
{}

Audio::~Audio()
{
    ndspExit();
}

Source * Audio::NewSource(Decoder * decoder)
{
    return new Source(decoder);
}

Source * Audio::NewSource(SoundData * sound)
{
    return new Source(sound);
}

bool Audio::Play(Source * source)
{
    return source->Play();
}

void Audio::Stop(Source * source)
{
    source->Stop();
}

void Audio::Stop()
{

}
