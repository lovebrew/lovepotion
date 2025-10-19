#pragma once

#include "common/Module.hpp"

#include "modules/audio/Source.hpp"
#include "modules/thread/Threadable.hpp"

#include "modules/audio/RecordingDevice.tcc"

#include <vector>

namespace love
{
    class AudioBase : public Module
    {
      public:
        enum AudioFormat
        {
            FORMAT_NONE,
            FORMAT_MONO8,
            FORMAT_MONO16,
            FORMAT_STEREO8,
            FORMAT_STEREO16
        };

        virtual ~AudioBase()
        {}

        virtual SourceBase* newSource(SoundData* soundData) const = 0;

        virtual SourceBase* newSource(Decoder* decoder) const = 0;

        virtual SourceBase* newSource(int sampleRate, int bitDepth, int channels, int buffers) const = 0;

        virtual int getActiveSourceCount() const = 0;

        virtual int getMaxSources() const = 0;

        virtual bool play(SourceBase* source) = 0;

        virtual bool play(const std::vector<SourceBase*>& sources) = 0;

        virtual void stop(SourceBase* source) = 0;

        virtual void stop(const std::vector<SourceBase*>& sources) = 0;

        virtual void stop() = 0;

        virtual void pause(SourceBase* source) = 0;

        virtual void pause(const std::vector<SourceBase*>& sources) = 0;

        virtual std::vector<SourceBase*> pause() = 0;

        virtual void setVolume(float volume) = 0;

        virtual float getVolume() const = 0;

        virtual const std::vector<RecordingDeviceBase*>& getRecordingDevices() = 0;

        static bool setMixWithSystem(bool mix);

      protected:
        AudioBase(const char* name);
    };
} // namespace love
