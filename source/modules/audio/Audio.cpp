#include "modules/audio/Audio.hpp"
#include "modules/audio/Pool.hpp"

#include "driver/audio/DigitalSound.hpp"

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
        DigitalSound::getInstance().initialize();

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

        DigitalSound::getInstance().deInitialize();

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

    bool Audio::play(const std::vector<Source*>& sources)
    {
        return Source::play(sources);
    }

    void Audio::stop()
    {
        Source::stop(this->pool);
    }

    void Audio::stop(Source* source)
    {
        source->stop();
    }

    void Audio::stop(const std::vector<Source*>& sources)
    {
        Source::stop(sources);
    }

    void Audio::pause(Source* source)
    {
        source->pause();
    }

    void Audio::pause(const std::vector<Source*>& sources)
    {
        Source::pause(sources);
    }

    std::vector<Source*> Audio::pause()
    {
        return Source::pause(this->pool);
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
