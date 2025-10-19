#include "common/Exception.hpp"
#include "common/config.hpp"
#include "common/int.hpp"

#include "driver/DigitalSound.tcc"

#include <coreinit/cache.h>
#include <coreinit/event.h>
#include <sndcore2/core.h>

#include <cstring>

extern "C"
{
    void AXSetMasterVolume(uint32_t volume);
    uint16_t AXGetMasterVolume();
}

namespace love
{
    namespace audio
    {

        // clang-format off
        static AXInitParams AX_INIT_PARAMS =
        {
            .renderer = AX_INIT_RENDERER_48KHZ,
            .pipeline = AX_INIT_PIPELINE_SINGLE
        };
        // clang-format on

        OSEvent s_Event;
        volatile bool s_Init = false;

        // # region Device

        bool Device::open()
        {
            AXInitWithParams(&AX_INIT_PARAMS);
            s_Init = AXIsInit();

            return s_Init;
        }

        void Device::update()
        {
            // TODO
        }

        void Device::close()
        {
            AXQuit();
        }

        void Device::setMasterVolume(float volume)
        {
            AXSetMasterVolume(volume * 0x8000);
        }

        float Device::getMasterVolume()
        {
            auto volume = AXGetMasterVolume();
            return volume / (float)0x8000;
        }

        // # endregion

        // #region Buffer

        struct UniqueVoiceScope
        {
            UniqueVoiceScope(AXVoice* voice) : voice(voice)
            {
                AXVoiceBegin(voice);
            }

            ~UniqueVoiceScope()
            {
                AXVoiceEnd(this->voice);
            }

          private:
            AXVoice* voice;
        };

        Buffer::Buffer(const size_t size, int channels)
        {
            if (size == 0)
                throw love::Exception("Audio buffer size cannot be zero.");

            this->buffer.data_pcm16 = (int16_t*)malloc(size);

            if (!this->buffer.data_pcm16)
                throw love::Exception(E_OUT_OF_MEMORY);

            for (int channel = 0; channel < channels; channel++)
            {
                this->buffer.voices[channel] = AXAcquireVoice(0x1F, nullptr, nullptr);

                UniqueVoiceScope scope(this->buffer.voices[channel]);
                AXSetVoiceType(this->buffer.voices[channel], AX_VOICE_TYPE_UNKNOWN);
            }

            this->buffer.channels = channels;
        }

        void Buffer::destroy()
        {
            for (auto* voice : this->buffer.voices)
                AXFreeVoice(voice);

            std::free(this->buffer.data_pcm16);
        }

        bool Buffer::isFinished() const
        {
            bool running = false;
            UniqueVoiceScope(this->buffer.voices[0]);
            running = AXIsVoiceRunning(this->buffer.voices[0]);
            return running;
        }

        void Buffer::prepare(const void* data, const size_t size, int samples)
        {
            if (data == nullptr || size == 0 || !this->buffer.data_pcm16)
                return;

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);

                AXVoiceOffsets offsets {};
                offsets.currentOffset = 0;
                offsets.endOffset     = samples;
                offsets.loopOffset    = 0;
                offsets.data          = this->buffer.data_pcm16;

                AXSetVoiceOffsets(this->buffer.voices[channel], &offsets);
                DCFlushRange(this->buffer.data_pcm16, size);
            }
        }

        void Buffer::set(const void* data, int samples)
        {
            this->buffer.data_pcm16 = (int16_t*)data;
            this->buffer.nsamples   = samples;
        }

        size_t Buffer::getSampleCount() const
        {
            UniqueVoiceScope(this->buffer.voices[0]);

            AXVoiceOffsets offsets {};
            AXGetVoiceOffsets(this->buffer.voices[0], &offsets);

            return offsets.currentOffset;
        }

        void Buffer::setLooping(bool looping)
        {
            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope(this->buffer.voices[channel]);

                AXVoiceOffsets offsets {};
                AXGetVoiceOffsets(this->buffer.voices[channel], &offsets);
                offsets.loopingEnabled = looping;

                AXSetVoiceOffsets(this->buffer.voices[channel], &offsets);
            }
        }

        bool Buffer::isLooping() const
        {
            UniqueVoiceScope(this->buffer.voices[0]);

            AXVoiceOffsets offsets {};
            AXGetVoiceOffsets(this->buffer.voices[0], &offsets);

            return offsets.loopingEnabled;
        }

        void Buffer::setStatus(uint8_t status)
        {}

        // #endregion

        // #region Channel

        bool Channel::reset(size_t id, int channels, int bitdepth, int samplerate, float volume)
        {
            return true;
        }

        void Channel::setVolume(size_t id, float volume)
        {}

        float Channel::getVolume(size_t id)
        {
            return 0.0f;
        }

        size_t Channel::getSampleOffset(size_t id)
        {
            return 0;
        }

        bool Channel::addBuffer(size_t id, Buffer& buffer)
        {
            return false;
        }

        void Channel::pause(size_t id, bool paused)
        {}

        bool Channel::isPaused(size_t id)
        {
            return false;
        }

        bool Channel::isPlaying(size_t id)
        {
            return false;
        }

        void Channel::stop(size_t id)
        {}
        // #endregion
    } // namespace audio
} // namespace love
