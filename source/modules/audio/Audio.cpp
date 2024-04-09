#include "modules/audio/Audio.hpp"
#include "modules/audio/Pool.hpp"

#include "driver/DigitalSound.hpp"

namespace love
{
    Audio::PoolThread::PoolThread(Pool* pool) : pool(pool), finish(false)
    {
        this->threadName = "AudioPool";
    }

    void Audio::PoolThread::run()
    {
        while (true)
        {
            if (this->finish)
                return;

            this->pool->update();
            DigitalSound::getInstance().update();
        }
    }

    Audio::Audio() : Module(M_AUDIO, "love.audio"), pool(nullptr), poolThread(nullptr)
    {
        try
        {
            this->pool = new Pool();
        }
        catch (love::Exception&)
        {
            throw;
        }

        this->poolThread = new PoolThread(this->pool);
        this->poolThread->start();
    }

    Audio::~Audio()
    {
        this->poolThread->setFinish();
        this->poolThread->wait();

        delete this->poolThread;
        delete this->pool;
    }

    Source* Audio::newSource(SoundData* soundData) const
    {
        return new Source(this->pool, soundData);
    }

    Source* Audio::newSource(Decoder* decoder) const
    {
        return new Source(this->pool, decoder);
    }

    Source* Audio::newSource(int sampleRate, int bitDepth, int channels, int buffers) const
    {
        return new Source(this->pool, sampleRate, bitDepth, channels, buffers);
    }

    int Audio::getActiveSourceCount() const
    {
        return this->pool->getActiveSourceCount();
    }

    bool Audio::play(Source* source)
    {
        return source->play();
    }

    void Audio::stop(Source* source)
    {
        source->stop();
    }

    void Audio::pause(Source* source)
    {
        source->pause();
    }

    void Audio::setVolume(float volume)
    {
        DigitalSound::getInstance().setMasterVolume(volume);
    }

    float Audio::getVolume() const
    {
        return DigitalSound::getInstance().getMasterVolume();
    }
} // namespace love
