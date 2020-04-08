#include "common/runtime.h"
#include "modules/audio/audio.h"
#include "modules/audio/pool/pool.h"

#include "audiodriver.h"

using namespace love;

std::atomic<bool> Audio::THREAD_RUN = false;

Audio::Audio()
{
    try
    {
        this->pool = new Pool();
    }
    catch (love::Exception &)
    {
        throw;
    }

    AudrenDriver::Initialize();

    THREAD_RUN = true;
    this->_Initialize();
}

Audio::~Audio()
{
    THREAD_RUN = false;
    this->_Exit();

    AudrenDriver::Exit();
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
