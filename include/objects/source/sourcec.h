#pragma once

#include "objects/decoder/decoder.h"
#include "objects/sounddata/sounddata.h"

#include "common/stringmap.h"

#include "modules/audio/pool/pool.h"
#include "objects/object.h"

namespace love
{
    class StaticDataBuffer : public Object
    {
      public:
        StaticDataBuffer(void* data, size_t size);

        virtual ~StaticDataBuffer();

        inline s16* GetBuffer() const
        {
            return this->buffer.first;
        }

        inline size_t GetSize() const
        {
            return this->buffer.second;
        }

      private:
        std::pair<s16*, size_t> buffer;
    };

    namespace common
    {
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

            Source(Pool* pool, SoundData* sound);

            Source(Pool* pool, Decoder* decoder);

            Source(const Source& other);

            virtual ~Source() {};

            virtual Source* Clone() = 0;

            bool Play();

            void Stop();

            void Pause();

            virtual bool IsPlaying() const = 0;

            virtual bool IsFinished() const = 0;

            Type GetType() const;

            int GetChannelCount() const;

            double GetDuration(Unit unit);

            int GetFreeBufferCount() const;

            float GetMinVolume() const;

            float GetMaxVolume() const;

            float GetVolume() const;

            virtual void SetVolume(float volume) = 0;

            void Seek(double offset, Unit unit);

            virtual void SetLooping(bool should) = 0;

            void SetVolumeLimits(float min, float max);

            void SetMinVolume(float volume);

            void SetMaxVolume(float volume);

            bool IsLooping() const;

            double Tell(Source::Unit unit);

            static bool Play(const std::vector<Source*>& sources);

            static void Stop(const std::vector<Source*>& sources);

            static void Pause(const std::vector<Source*>& sources);

            static std::vector<Source*> Pause(Pool* pool);

            static void Stop(Pool* pool);

            static bool GetConstant(const char* in, Type& out);
            static bool GetConstant(Type in, const char*& out);
            static std::vector<const char*> GetConstants(Type);

            static bool GetConstant(const char* in, Unit& out);
            static bool GetConstant(Unit in, const char*& out);
            static std::vector<const char*> GetConstants(Unit);

            virtual bool Update() = 0;

            virtual void StopAtomic() = 0;

          protected:
            Type sourceType;

            constexpr static int DEFAULT_BUFFERS = 2;
            constexpr static int MAX_BUFFERS     = 2;

            void* sourceBuffer;
            size_t souceBufferSize;

            bool valid = false;

            float volume = 1.0f;

            bool looping = false;

            float minVolume = 0.0f;
            float maxVolume = 1.0f;

            std::atomic<int> offsetSamples = 0;

            int sampleRate = 0;
            int channels   = 0;
            int bitDepth   = 0;

            Pool* pool = nullptr;

            size_t channel = 0;

            virtual void InitializeStreamBuffers(Decoder* decoder) = 0;

            StrongReference<Decoder> decoder;
            StrongReference<StaticDataBuffer> staticBuffer;

            virtual void ClearChannel() = 0;

            virtual void FreeBuffer() = 0;

            virtual void Reset() = 0;

            virtual void PrepareAtomic() = 0;

            virtual int StreamAtomic(size_t which) = 0;

            virtual bool PlayAtomic() = 0;

            virtual void PauseAtomic() = 0;

            virtual void ResumeAtomic() = 0;

            virtual double GetSampleOffset() = 0;

            void TeardownAtomic();

          private:
            const static StringMap<Type, TYPE_MAX_ENUM> types;
            const static StringMap<Unit, UNIT_MAX_ENUM> units;
        };
    } // namespace common
} // namespace love
