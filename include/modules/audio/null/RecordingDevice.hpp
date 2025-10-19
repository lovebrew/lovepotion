#pragma once

#include "modules/audio/RecordingDevice.tcc"
#include "modules/sound/SoundData.hpp"

namespace love
{
    namespace audio
    {
        namespace null
        {
            class RecordingDevice final : public RecordingDeviceBase
            {
              public:
                RecordingDevice(const char* name);

                virtual ~RecordingDevice();

                virtual bool start(int samples, int sampleRate, int bitDepth, int channels);

                virtual void stop();

                virtual SoundData* getData();

                virtual const char* getName() const;

                virtual int getMaxSamples() const;

                virtual int getSampleCount() const;

                virtual int getSampleRate() const;

                virtual int getBitDepth() const;

                virtual int getChannelCount() const;

                virtual bool isRecording() const;

              private:
                static const char* name;
            };
        } // namespace null
    } // namespace audio
} // namespace love
