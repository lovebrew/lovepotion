#pragma once

#include "common/Map.hpp"
#include "common/Singleton.tcc"

#include <array>
#include <mutex>

#if defined(__3DS__)
    #include <3ds.h>
using AudioBuf   = ndspWaveBuf;
using NdspFormat = decltype(NDSP_FORMAT_MONO_PCM8);
#elif defined(__SWITCH__)
    #include <switch.h>
using AudioBuf = AudioDriverWaveBuf;
#else
    #include <sndcore2/voice.h>
struct AudioBuf
{
    std::array<AXVoice*, 2> voices;
    int16_t* data_pcm16;
    size_t nsamples;
    uint32_t offset;
    bool looping;
    int channels;
};
#endif

namespace love
{
    namespace audio
    {
        class Device final
        {
          public:
            static bool open();

            static void close();

            static void update();

            static void setMasterVolume(float volume);

            static float getMasterVolume();

            static std::recursive_mutex& getMutex()
            {
                static std::recursive_mutex s_Mutex;
                return s_Mutex;
            }
        };

        class Buffer final
        {
          public:
            Buffer()
            {}

            Buffer(const size_t size, int channels);

            void destroy();

            bool isFinished() const;

            void prepare(const void* data, const size_t size, int samples, bool own = true);

            size_t getSampleCount() const;

            void setLooping(bool looping);

            bool isLooping() const;

            void setStatus(uint8_t status);

            AudioBuf* getHandle()
            {
                return std::addressof(this->buffer);
            };

            void* getData() const
            {
                return this->buffer.data_pcm16;
            }

          private:
            AudioBuf buffer;
        };

        class Channel final
        {
          public:
            static constexpr size_t MAX_CHANNELS = 24;

            static bool reset(size_t id, int channels, int bitdepth, int samplerate, float volume);

            static void setVolume(size_t id, float volume);

            static float getVolume(size_t id);

            static size_t getSampleOffset(size_t id);

            static bool addBuffer(size_t id, Buffer& buffer);

            static void pause(size_t id, bool paused);

            static bool isPaused(size_t id);

            static bool isPlaying(size_t id);

            static void stop(size_t id);
        };
    } // namespace audio
} // namespace love
