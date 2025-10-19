#pragma once

#include "common/Object.hpp"
#include "modules/audio/Source.hpp"

#include <limits>

namespace love
{
    namespace audio
    {
        namespace null
        {
            class Source final : public SourceBase
            {
              public:
                Source();

                virtual ~Source();

                virtual SourceBase* clone();

                virtual bool play();

                virtual void stop();

                virtual void pause();

                virtual bool isPlaying() const;

                virtual bool isFinished() const;

                virtual bool update();

                virtual void setPitch(float pitch);

                virtual float getPitch() const;

                virtual void setVolume(float volume);

                virtual float getVolume() const;

                virtual void seek(double offset, Unit unit);

                virtual double tell(Unit unit);

                virtual double getDuration(Unit unit);

                void setLooping(bool looping);

                bool isLooping() const;

                virtual bool queue(void* data, size_t length, int samplerate, int bitdepth, int channels);

                virtual void setMinVolume(float volume);

                virtual float getMinVolume() const;

                virtual void setMaxVolume(float volume);

                virtual float getMaxVolume() const;

                virtual int getChannelCount() const;

                virtual int getFreeBufferCount() const;

              private:
                float pitch     = 1.0f;
                float volume    = 1.0f;
                bool looping    = false;
                float minVolume = 0.0f;
                float maxVolume = 1.0f;
            };
        } // namespace null
    } // namespace audio
} // namespace love
