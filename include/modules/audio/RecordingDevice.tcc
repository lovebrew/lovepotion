#pragma once

#include "common/Object.hpp"
#include "modules/sound/SoundData.hpp"

#include <string>

namespace love
{
    class RecordingDeviceBase : public Object
    {
      public:
        static Type type;

        static constexpr int DEFAULT_SAMPLES     = 8192;
        static constexpr int DEFAULT_SAMPLE_RATE = 8000;
        static constexpr int DEFAULT_BIT_DEPTH   = 16;
        static constexpr int DEFAULT_CHANNELS    = 1;

        RecordingDeviceBase();

        virtual ~RecordingDeviceBase();

        virtual bool start(int samples, int sampleRate, int bitDepth, int channels) = 0;

        virtual void stop() = 0;

        virtual SoundData* getData() = 0;

        virtual const char* getName() const = 0;

        virtual int getSampleCount() const = 0;

        virtual int getMaxSamples() const = 0;

        virtual int getSampleRate() const = 0;

        virtual int getBitDepth() const = 0;

        virtual int getChannelCount() const = 0;

        virtual bool isRecording() const = 0;

        virtual void setGain(uint8_t gain) = 0;

        virtual uint8_t getGain() const = 0;

      protected:
        class InvalidFormatException : public love::Exception
        {
          public:
            InvalidFormatException(int channels, int bitdepth) :
                Exception("Recording %d channels with %d bits per sample is not supported.", channels,
                          bitdepth)
            {}
        };
    };
} // namespace love
