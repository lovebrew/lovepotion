#include "common/Exception.hpp"
#include "common/config.hpp"
#include "common/debug.hpp"
#include "common/int.hpp"

#include "driver/DigitalSound.tcc"
#include "driver/audio/DigitalSoundMix.hpp"

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
            OSSleepTicks(OSMillisecondsToTicks(3));
        }

        void Device::close()
        {
            if (!s_Init)
                return;

            s_Init = false;
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

            UniqueVoiceScope(UniqueVoiceScope&&) = delete;

            UniqueVoiceScope& operator=(UniqueVoiceScope&&) = delete;

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
            this->buffer.paused   = false;
            this->buffer.size     = size;
        }

        Buffer Buffer::clone()
        {
            Buffer view {};
            view.buffer = this->buffer;

            return view;
        }

        void Buffer::destroy()
        {
            for (auto* voice : this->buffer.voices)
            {
                if (voice)
                    AXFreeVoice(voice);
            }

            std::free(this->buffer.data_pcm16);
        }

        bool Buffer::isFinished() const
        {
            const auto running = AXIsVoiceRunning(this->buffer.voices[0]);
            return running == false;
        }

        void Buffer::prepare(const void* data, size_t size, int samples, bool own)
        {
            if (data == nullptr || size == 0 || !this->buffer.data_pcm16)
                return;

            if (own)
            {
                if (!this->buffer.data_pcm16)
                    return;

                std::memcpy(this->buffer.data_pcm16, data, size);
            }
            else
                this->buffer.data_pcm16 = (int16_t*)data;

            DCStoreRange(this->buffer.data_pcm16, size);

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);

                AXVoiceOffsets offsets {};
                offsets.currentOffset = 0;
                offsets.endOffset     = samples;
                offsets.loopOffset    = 0;
                offsets.data          = this->buffer.data_pcm16;
                offsets.dataType      = AX_VOICE_FORMAT_LPCM16;

                AXSetVoiceOffsets(this->buffer.voices[channel], &offsets);
            }
        }

        size_t Buffer::getSampleCount() const
        {
            AXVoiceOffsets offsets {};
            AXGetVoiceOffsets(this->buffer.voices[0], &offsets);
            return offsets.currentOffset;
        }

        void Buffer::setLooping(bool looping)
        {
            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);
                const auto loop = looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;
                AXSetVoiceLoop(this->buffer.voices[channel], loop);
            }
        }

        bool Buffer::isLooping() const
        {
            AXVoiceOffsets offsets {};
            AXGetVoiceOffsets(this->buffer.voices[0], &offsets);

            return offsets.loopingEnabled;
        }

        void Buffer::setStatus(uint8_t status)
        {
            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);
                AXSetVoiceState(this->buffer.voices[channel], status);
            }
        }

        void Buffer::setSampleRate(int samplerate)
        {
            this->buffer.samplerate = samplerate;
            const auto ratio        = (float)samplerate / (float)AXGetInputSamplesPerSec();
            LOG("Setting src ratio to %f", ratio);

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);
                AXSetVoiceSrcRatio(this->buffer.voices[channel], ratio);
                AXSetVoiceSrcType(this->buffer.voices[channel], AX_VOICE_SRC_TYPE_LINEAR);
            }
        }

        bool Buffer::isPaused() const
        {
            return this->buffer.paused;
        }

        void Buffer::setPaused(bool paused)
        {
            this->buffer.paused = paused;
        }

        void Buffer::setVolume(float volume)
        {
            const auto ve      = static_cast<uint16_t>(volume * 0x8000u);
            AXVoiceVeData data = { .volume = ve, .delta = 0 };
            LOG("Setting buffer volume to %f (VE: %u)", volume, ve);

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);
                AXSetVoiceVe(this->buffer.voices[channel], &data);
            }
        }

        float Buffer::getVolume() const
        {
            return this->buffer.voices[0]->volume / (float)0x8000;
        }

        // #endregion

        // #region Channel

        static std::array<Buffer, Channel::MAX_CHANNELS> s_Channels;

        bool Channel::reset(size_t id, int channels, int bitdepth, int samplerate, float volume)
        {
            LOG("Resetting channel %zu with %d channels, %d bitdepth, %d samplerate, volume %f", id, channels,
                bitdepth, samplerate, volume);
            auto& buffer = s_Channels[id];

            buffer.setStatus(AX_VOICE_STATE_STOPPED);
            buffer.setSampleRate(samplerate);
            buffer.setVolume(volume);

            return true;
        }

        void Channel::setVolume(size_t id, float volume)
        {
            auto& buffer = s_Channels[id];
            buffer.setVolume(volume);
        }

        float Channel::getVolume(size_t id)
        {
            auto& buffer = s_Channels[id];
            return buffer.getVolume();
        }

        size_t Channel::getSampleOffset(size_t id)
        {
            auto& buffer = s_Channels[id];
            return buffer.getSampleCount();
        }

        bool Channel::addBuffer(size_t id, Buffer& buffer)
        {
            LOG("Adding buffer to channel %zu", id);
            const auto channels = buffer.getHandle()->channels;

            AXVoiceDeviceMixData mix[6] {};
            std::memset(mix, 0, sizeof(mix));

            mix[0].bus[0].volume = 0x8000;
            if (channels == 2)
                mix[1].bus[0].volume = 0x8000;
            LOG("Setting voice device mix for channel %d", channels);
            for (int channel = 0; channel < channels; channel++)
            {
                auto* voice = buffer.getHandle()->voices[channel];
                UniqueVoiceScope scope(voice);

                AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, mix);
                AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, mix);
            }
            LOG("Setting voice state to playing for channel %d", channels);
            buffer.setStatus(AX_VOICE_STATE_PLAYING);
            buffer.setPaused(false);
            buffer.setSampleRate(buffer.getHandle()->samplerate);
            s_Channels[id] = buffer;
            return true;
        }

        void Channel::pause(size_t id, bool paused)
        {
            LOG("Pausing channel %zu: %s", id, paused ? "true" : "false");
            auto& buffer = s_Channels[id];

            buffer.setPaused(paused);
            buffer.setStatus(paused ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING);
        }

        bool Channel::isPaused(size_t id)
        {
            auto& buffer = s_Channels[id];
            return buffer.isPaused();
        }

        bool Channel::isPlaying(size_t id)
        {
            auto& buffer = s_Channels[id];
            return !buffer.isFinished();
        }

        void Channel::stop(size_t id)
        {
            LOG("Stopping channel %zu", id);
            auto& buffer = s_Channels[id];
            buffer.setStatus(AX_VOICE_STATE_STOPPED);
        }
        // #endregion
    } // namespace audio
} // namespace love
