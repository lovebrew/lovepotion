#pragma once

#include "modules/audio/Audio.hpp"
#include "modules/audio/null/RecordingDevice.hpp"
#include "modules/audio/null/Source.hpp"

namespace love
{
    namespace audio
    {
        namespace null
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

              private:
                float volume;
                std::vector<RecordingDeviceBase*> capture;
            };
        } // namespace null
    } // namespace audio
} // namespace love
