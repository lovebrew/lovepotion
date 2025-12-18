#include "common/Exception.hpp"
#include "common/config.hpp"
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
            // TODO
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
                AXSetVoiceType(this->buffer.voices[channel], AX_VOICE_SRC_TYPE_LINEAR);
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
                AXFreeVoice(voice);

            std::free(this->buffer.data_pcm16);
        }

        bool Buffer::isFinished() const
        {
            return !AXIsVoiceRunning(this->buffer.voices[0]);
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

            DCFlushRange(this->buffer.data_pcm16, size);
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

                AXVoiceOffsets offsets {};
                AXGetVoiceOffsets(this->buffer.voices[channel], &offsets);
                offsets.loopingEnabled = looping;
                AXSetVoiceOffsets(this->buffer.voices[channel], &offsets);
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

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);
                float ratio = (float)samplerate / (float)AXGetInputSamplesPerSec();
                AXSetVoiceSrcRatio(this->buffer.voices[channel], ratio);
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
            AXVoiceVeData data = { .volume = volume * 0x8000 };

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
            const auto channels = buffer.getHandle()->channels;

            for (int channel = 0; channel < channels; channel++)
            {
                auto* voice = buffer.getHandle()->voices[channel];
                UniqueVoiceScope scope(voice);

                switch (channels)
                {
                    case 1:
                    {
                        AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, MONO_MIX[channel]);
                        AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, MONO_MIX[channel]);
                        break;
                    }
                    case 2:
                    {
                        AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, STEREO_MIX[channel]);
                        AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, STEREO_MIX[channel]);
                        break;
                    }
                    default:
                        break;
                }

                AXSetVoiceState(voice, AX_VOICE_STATE_PLAYING);
            }

            s_Channels[id] = buffer;
            return true;
        }

        void Channel::pause(size_t id, bool paused)
        {
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
            auto& buffer = s_Channels[id];
            buffer.setStatus(AX_VOICE_STATE_STOPPED);
        }
        // #endregion
    } // namespace audio
} // namespace love
