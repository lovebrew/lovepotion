#pragma once

#include "objects/decoder/decoder.h"
#include "objects/sounddata/sounddata.h"
#include "common/stringmap.h"

namespace love
{
    class StaticDataBuffer : public Object
    {
        public:
            StaticDataBuffer(void * data, size_t size);
            ~StaticDataBuffer();

            inline s16 * GetBuffer() const
            {
                return this->buffer;
            }

            inline size_t GetSize() const
            {
                return this->size;
            }

        private:
            s16 * buffer;
            size_t size;
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

            Source(SoundData * sound);
            Source(Decoder * decoder);
            Source(const Source & other);

            ~Source();

            Source * Clone();

            bool Play();

            void Stop();

            void Pause();

            bool IsPlaying() const;

            bool IsFinished() const;

            void SetVolume(float volume);

            float GetVolume() const;

            Type GetType() const;

            static bool GetConstant(const char * in, Type & out);
            static bool GetConstant(Type in, const char  *& out);
            static std::vector<std::string> GetConstants(Type);

        protected:
            Type sourceType;

        private:
            void Reset();

            void PrepareAtomic();
            int StreamAtomic(void * buffer, Decoder * decoder);
            bool PlayAtomic();
            void ResumeAtomic();

            bool valid = false;

            const static int DEFAULT_BUFFERS = 8;
            const static int MAX_BUFFERS = 64;

            float volume = 1.0f;
            bool looping = false;
            float minVolume = 0.0f;
            float maxVolume = 1.0f;

            int offsetSamples = 0;

            int sampleRate = 0;
            int channels = 0;
            int bitDepth = 0;

            StrongReference<Decoder> decoder;
            StrongReference<StaticDataBuffer> staticBuffer;

            int buffers = 0;

            std::stack<s16 *> unusedBuffers;
            std::queue<s16 *> streamBuffers;

            ndspWaveBuf source;

            static StringMap<Type, TYPE_MAX_ENUM>::Entry typeEntries[];
            static StringMap<Type, TYPE_MAX_ENUM> types;
    };
}
