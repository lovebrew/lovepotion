#include "driver/DigitalSound.tcc"

#include "modules/audio/dsp/Audio.hpp"

#include "modules/audio/RecordingDevice.hpp"

namespace love
{
    namespace audio
    {
        namespace dsp
        {
            Audio::PoolThread::PoolThread(Pool* pool) : pool(pool), finish(false)
            {
                this->threadName = "AudioPool";
            }

            Audio::PoolThread::~PoolThread()
            {}

            void Audio::PoolThread::run()
            {
                while (!this->finish)
                {
                    this->pool->update();
                    audio::Device::update();
                }
            }

            void Audio::PoolThread::setFinish()
            {
                this->finish = true;
            }

            Audio::AudioFormat Audio::getFormat(int bitdepth, int channels)
            {
                if (bitdepth != 8 && bitdepth != 16)
                    return FORMAT_NONE;

                if (channels == 1)
                    return bitdepth == 8 ? FORMAT_MONO8 : FORMAT_MONO16;
                else if (channels == 2)
                    return bitdepth == 8 ? FORMAT_STEREO8 : FORMAT_STEREO16;

                return FORMAT_NONE;
            }

            Audio::Audio() : AudioBase("love.audio.dsp"), pool(nullptr), poolThread(nullptr)
            {
                if (!audio::Device::open())
                    throw love::Exception("Could not open device.");

                try
                {
                    this->pool = new Pool();
                }
                catch (love::Exception&)
                {
                    for (auto* c : this->capture)
                        delete c;

                    audio::Device::close();
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

                for (auto c : this->capture)
                    delete c;

                audio::Device::close();
            }

            SourceBase* Audio::newSource(SoundData* data) const
            {
                return new Source(this->pool, data);
            }

            SourceBase* Audio::newSource(Decoder* decoder) const
            {
                return new Source(this->pool, decoder);
            }

            SourceBase* Audio::newSource(int samplerate, int bitdepth, int channels, int buffers) const
            {
                return new Source(this->pool, samplerate, bitdepth, channels, buffers);
            }

            int Audio::getActiveSourceCount() const
            {
                return this->pool->getActiveSourceCount();
            }

            bool Audio::play(SourceBase* source)
            {
                return source->play();
            }

            bool Audio::play(const std::vector<SourceBase*>& sources)
            {
                return Source::play(sources);
            }

            void Audio::stop()
            {
                Source::stop(this->pool);
            }

            void Audio::stop(SourceBase* source)
            {
                source->stop();
            }

            void Audio::stop(const std::vector<SourceBase*>& sources)
            {
                Source::stop(sources);
            }

            void Audio::pause(SourceBase* source)
            {
                source->pause();
            }

            void Audio::pause(const std::vector<SourceBase*>& sources)
            {
                Source::pause(sources);
            }

            std::vector<SourceBase*> Audio::pause()
            {
                return Source::pause(this->pool);
            }

            void Audio::setVolume(float volume)
            {
                audio::Device::setMasterVolume(volume);
            }

            float Audio::getVolume() const
            {
                return audio::Device::getMasterVolume();
            }

            const std::vector<RecordingDeviceBase*>& Audio::getRecordingDevices()
            {
                std::vector<RecordingDeviceBase*> devices;
                std::vector<std::string> deviceNames = { "Default Device Microphone" };

                devices.reserve(deviceNames.size());
                for (size_t index = 0; index < deviceNames.size(); index++)
                {
                    devices.push_back(nullptr);
                    auto device = devices.end() - 1;

                    for (auto* c : this->capture)
                    {
                        if (deviceNames[index] == c->getName())
                        {
                            *device = c;
                            break;
                        }
                    }

                    if (*device == nullptr)
                        *device = new RecordingDevice(deviceNames[index].c_str());
                    else
                        (*device)->retain();
                }

                for (auto* c : this->capture)
                    c->release();

                this->capture.clear();
                this->capture.reserve(devices.size());

                for (size_t index = 0; index < deviceNames.size(); index++)
                    this->capture.push_back(devices[index]);

                return this->capture;
            }

            int Audio::getMaxSources() const
            {
                return Channel::MAX_CHANNELS;
            }
        } // namespace dsp
    } // namespace audio
} // namespace love
