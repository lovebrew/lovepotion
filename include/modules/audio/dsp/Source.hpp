#pragma once

#include "common/Object.hpp"

#include "modules/audio/Source.hpp"
#include "modules/audio/dsp/Audio.hpp"
#include "modules/sound/Decoder.hpp"
#include "modules/sound/SoundData.hpp"

#include <queue>
#include <stack>

namespace love
{
    namespace audio
    {
        namespace dsp
        {
            class Audio;
            class Pool;

            class StaticDataBuffer : public Object
            {
              public:
                StaticDataBuffer(const SoundData* data);

                ~StaticDataBuffer();

                void setLooping(bool looping);

                void* getBuffer() const;

                size_t getSize() const;

                audio::Buffer& getView(const size_t samples, int channels);

              private:
                size_t size;
                size_t nsamples;

                audio::Buffer buffer;
                audio::Buffer view;
            };

            class Source final : public SourceBase
            {
              public:
                Source(Pool* pool, SoundData* data);

                Source(Pool* pool, Decoder* decoder);

                Source(Pool* pool, int samplerate, int bitdepth, int channels, int buffers);

                Source(const Source& source);

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

                void prepareAtomic();

                void teardownAtomic();

                bool playAtomic();

                void stopAtomic();

                void pauseAtomic();

                void resumeAtomic();

                static bool play(const std::vector<SourceBase*>& sources);

                static void stop(const std::vector<SourceBase*>& sources);

                static void pause(const std::vector<SourceBase*>& sources);

                static std::vector<SourceBase*> pause(Pool* pool);

                static void stop(Pool* pool);

              private:
                void reset();

                int streamAtomic(audio::Buffer& buffer, Decoder* decoder);

                Pool* pool = nullptr;
                bool valid = false;

#if defined(__3DS__)
                static constexpr int DEFAULT_BUFFERS = 2;
                static constexpr int MAX_BUFFERS     = 4;
#else
                static constexpr int DEFAULT_BUFFERS = 8;
                static constexpr int MAX_BUFFERS     = 64;
#endif
                std::vector<audio::Buffer> streamBuffers;
                std::stack<audio::Buffer> unusedBuffers;

                StrongRef<StaticDataBuffer> staticBuffer;

                float pitch     = 1.0f;
                float volume    = 1.0f;
                bool looping    = false;
                float minVolume = 0.0f;
                float maxVolume = 1.0f;

                int offsetSamples = 0;
                int samplerate    = 0;
                int channels      = 0;
                int bitdepth      = 0;

                StrongRef<Decoder> decoder;
                int buffers = 0;

                size_t channel = 0;
            };
        } // namespace dsp
    } // namespace audio
} // namespace love
