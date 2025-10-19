#pragma once

#include "modules/audio/Audio.hpp"
#include "modules/audio/RecordingDevice.tcc"
#include "modules/sound/SoundData.hpp"

#include "modules/audio/dsp/Pool.hpp"
#include "modules/audio/dsp/Source.hpp"

#include <cmath>
#include <vector>

namespace love
{
    namespace audio
    {
        namespace dsp
        {
            class Audio final : public AudioBase
            {
              public:
                Audio();

                virtual ~Audio();

                SourceBase* newSource(Decoder* decoder) const;

                SourceBase* newSource(SoundData* data) const;

                SourceBase* newSource(int samplerate, int bitdepth, int channels, int buffers) const;

                int getActiveSourceCount() const;

                int getMaxSources() const;

                bool play(SourceBase* source);

                bool play(const std::vector<SourceBase*>& sources);

                void stop(SourceBase* source);

                void stop(const std::vector<SourceBase*>& sources);

                void stop();

                void pause(SourceBase* source);

                void pause(const std::vector<SourceBase*>& sources);

                std::vector<SourceBase*> pause();

                void setVolume(float volume);

                float getVolume() const;

                const std::vector<RecordingDeviceBase*>& getRecordingDevices();

                static AudioFormat getFormat(int bitdepth, int channels);

              private:
                std::vector<RecordingDeviceBase*> capture;
                Pool* pool;

                class PoolThread : public Threadable
                {
                  public:
                    PoolThread(Pool* pool);

                    virtual ~PoolThread();

                    void setFinish();

                    void run();

                  protected:
                    Pool* pool;
                    std::atomic<bool> finish;
                };

                PoolThread* poolThread;
            };
        } // namespace dsp
    } // namespace audio
} // namespace love
