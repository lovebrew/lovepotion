#pragma once

#include "common/Object.hpp"
#include "driver/DigitalSound.tcc"

#include "modules/sound/Decoder.hpp"
#include "modules/sound/SoundData.hpp"

#include <vector>

namespace love
{
    class SourceBase : public Object
    {
      public:
        static love::Type type;

        enum Type
        {
            TYPE_STATIC,
            TYPE_STREAM,
            TYPE_QUEUE,
            TYPE_MAX_ENUM
        };

        enum Unit
        {
            UNIT_SECONDS,
            UNIT_SAMPLES,
            UNIT_MAX_ENUM
        };

        SourceBase(Type type);

        virtual ~SourceBase();

        virtual SourceBase* clone() = 0;

        virtual bool play() = 0;

        virtual void stop() = 0;

        virtual void pause() = 0;

        virtual bool isPlaying() const = 0;

        virtual bool isFinished() const = 0;

        virtual bool update() = 0;

        virtual void setPitch(float pitch) = 0;

        virtual float getPitch() const = 0;

        virtual void setVolume(float volume) = 0;

        virtual float getVolume() const = 0;

        virtual void seek(double offset, Unit unit) = 0;

        virtual double tell(Unit unit) = 0;

        virtual double getDuration(Unit unit) = 0;

        virtual void setLooping(bool looping) = 0;

        virtual bool isLooping() const = 0;

        virtual void setMinVolume(float minVolume) = 0;

        virtual float getMinVolume() const = 0;

        virtual void setMaxVolume(float maxVolume) = 0;

        virtual float getMaxVolume() const = 0;

        virtual int getChannelCount() const = 0;

        virtual int getFreeBufferCount() const = 0;

        virtual bool queue(void* data, size_t length, int samplerate, int bitdepth, int channels) = 0;

        virtual Type getType() const;

        // clang-format off
        STRINGMAP_DECLARE(SourceTypes, Type,
            { "static", TYPE_STATIC },
            { "stream", TYPE_STREAM },
            { "queue",  TYPE_QUEUE  }
        );

        STRINGMAP_DECLARE(SourceUnits, Unit,
            { "seconds", UNIT_SECONDS },
            { "samples", UNIT_SAMPLES }
        );
        // clang-format on

      protected:
        Type sourceType;
    };
} // namespace love
