#pragma once

#include "objects/decoder/decoder.h"
#include "objects/sounddata/sounddata.h"
#include "common/stringmap.h"

#include "modules/audio/pool/pool.h"

#if defined(_3DS)
    typedef ndspWaveBuf _waveBuf;
    #define LOVE_SetBufferLooping(waveBuf, loop) ((waveBuf).looping = (loop))
    #define FlushAudioCache DSP_FlushDataCache
#elif defined (__SWITCH__)
    typedef AudioDriverWaveBuf _waveBuf;
    #define LOVE_SetBufferLooping(waveBuf, loop) ((waveBuf).is_looping = (loop))
    #define FlushAudioCache armDCacheFlush
#endif

namespace love
{
    class StaticDataBuffer : public Object
    {
        public:
            StaticDataBuffer(void * data, size_t size);
            ~StaticDataBuffer();

            inline s16 * GetBuffer() const
            {
                return this->buffer.first;
            }

            inline size_t GetSize() const
            {
                return this->buffer.second;
            }

        private:
            std::pair<s16 *, size_t> buffer;
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

            Source(Pool * pool, SoundData * sound);
            Source(Pool * pool, Decoder * decoder);

            Source(const Source & other);

            ~Source();

            Source * Clone();

            bool Play();

            void Stop();

            void Pause();

            bool IsPlaying() const;

            bool IsFinished() const;

            Type GetType() const;

            int GetChannelCount() const;

            double GetDuration(Unit unit);

            int GetFreeBufferCount() const;

            float GetMinVolume() const;

            float GetMaxVolume() const;

            float GetVolume() const;

            void SetVolume(float volume);

            void Seek(double offset, Unit unit);

            void SetLooping(bool loop);

            void SetVolumeLimits(float min, float max);

            void SetMinVolume(float volume);

            void SetMaxVolume(float volume);

            bool IsLooping() const;

            double Tell();

            static bool GetConstant(const char * in, Type & out);
            static bool GetConstant(Type in, const char  *& out);
            static std::vector<std::string> GetConstants(Type);

            static bool GetConstant(const char * in, Unit & out);
            static bool GetConstant(Unit in, const char  *& out);
            static std::vector<std::string> GetConstants(Unit);

            bool Update();

            void AddWaveBuffer(size_t index);
            void StopAtomic();


        protected:
            Type sourceType;

        private:
            void Reset();

            void CreateWaveBuffer(SoundData * sound);
            void CreateWaveBuffer(Decoder * decoder);

            void FreeBuffer();

            bool _Update();
            void _Prepare(size_t which, size_t decoded);
            void _SetLooping(bool shouldLoop);

            void PrepareAtomic();
            int StreamAtomic(s16 * buffer, Decoder * decoder);
            bool PlayAtomic();
            void TeardownAtomic();
            void ResumeAtomic();
            void PauseAtomic();

            int GetLastIndex();

            Pool * pool = nullptr;

            bool valid = false;

            const static int DEFAULT_BUFFERS = 2;
            const static int MAX_BUFFERS = 2;

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

            _waveBuf sources[DEFAULT_BUFFERS];

            bool index = false;
            size_t channel = 0;

            static StringMap<Type, TYPE_MAX_ENUM>::Entry typeEntries[];
            static StringMap<Type, TYPE_MAX_ENUM> types;

            static StringMap<Unit, UNIT_MAX_ENUM>::Entry unitEntries[];
            static StringMap<Unit, UNIT_MAX_ENUM> units;
    };
}
