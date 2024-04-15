#pragma once

#include "common/Object.hpp"
#include "driver/audio/DigitalSound.hpp"

#include "modules/sound/Decoder.hpp"
#include "modules/sound/SoundData.hpp"

#include <queue>
#include <stack>
#include <vector>

namespace love
{
    class Audio;
    class Pool;

    class StaticDataBuffer : public Object
    {
      public:
        StaticDataBuffer(const SoundData* data);

        virtual ~StaticDataBuffer();

        size_t getSampleCount() const
        {
            return this->nsamples;
        }

        AudioBuffer& clone(const size_t offsetSamples, const int channels);

        void* getBuffer() const;

        size_t getSize() const
        {
            return this->size;
        }

        void setLooping(bool looping);

      private:
        AudioBuffer buffer;
        AudioBuffer _clone;

        size_t size;
        int nsamples;
    };

    class Source : public Object
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

        Source(Type type) : sourceType(type)
        {}

        Source(Pool* pool, SoundData* soundData);

        Source(Pool* pool, Decoder* decoder);

        Source(Pool* pool, int sampleRate, int bitDepth, int channels, int buffers);

        Source(const Source& other);

        virtual ~Source();

        virtual Source* clone();

        bool play();

        void stop();

        void pause();

        bool isPlaying() const;

        bool isFinished() const;

        bool update();

        void setPitch(float pitch)
        {
            this->pitch = pitch;
        }

        float getPitch() const
        {
            return this->pitch;
        }

        void setVolume(float volume);

        float getVolume() const;

        void seek(double offset, Unit unit);

        double tell(Unit unit);

        double getDuration(Unit unit);

        void setLooping(bool looping);

        bool isLooping() const;

        int getChannelCount() const
        {
            return this->channels;
        }

        Type getType() const
        {
            return this->sourceType;
        }

        void setMinVolume(float minVolume)
        {
            this->minVolume = minVolume;
        }

        float getMinVolume() const
        {
            return this->minVolume;
        }

        void setMaxVolume(float maxVolume)
        {
            this->maxVolume = maxVolume;
        }

        float getMaxVolume() const
        {
            return this->maxVolume;
        }

        int getFreeBufferCount() const;

        void prepareAtomic();

        void teardownAtomic();

        bool playAtomic();

        void stopAtomic();

        void pauseAtomic();

        void resumeAtomic();

        static bool play(const std::vector<Source*>& sources);

        static void stop(const std::vector<Source*>& sources);

        static void pause(const std::vector<Source*>& sources);

        static std::vector<Source*> pause(Pool* pool);

        static void stop(Pool* pool);

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

      private:
        void reset();

        int streamAtomic(AudioBuffer& buffer, Decoder* decoder);

        Type sourceType;
        Pool* pool = nullptr;

        AudioBuffer* source = nullptr;
        bool valid          = false;

#if !defined(__3DS__)
        static constexpr int DEFAULT_BUFFERS = 8;
        static constexpr int MAX_BUFFERS     = 64;
#else
        /*
        ** 3DS linearMem is only like 16MB.
        ** Let's not go overboard with the buffers.
        ** Since we need some of that for other things.
        */
        static constexpr int DEFAULT_BUFFERS = 2;
        static constexpr int MAX_BUFFERS     = 4;
#endif

        std::vector<AudioBuffer> streamBuffers;
        StrongRef<StaticDataBuffer> staticBuffer;

        float pitch  = 1.0f;
        float volume = 1.0f;
        bool looping = false;

        float minVolume = 0.0f;
        float maxVolume = 1.0f;

        int offsetSamples = 0;

        int sampleRate = 0;
        int channels   = 0;
        int bitDepth   = 0;

        int buffers = 0;

        StrongRef<Decoder> decoder;

        size_t channel = 0;
    };
} // namespace love
