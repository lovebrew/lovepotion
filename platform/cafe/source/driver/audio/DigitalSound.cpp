#include "common/Exception.hpp"
#include "common/config.hpp"
#include "common/debug.hpp"
#include "common/int.hpp"

#include "modules/audio/dsp/Audio.hpp"

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
             AX_INIT_RENDERER_48KHZ,
             AX_INIT_PIPELINE_SINGLE,
             0
        };
        // clang-format on

        OSEvent s_Event;
        volatile bool s_Init = false;

        static void audioCallback()
        {
            if (!s_Init)
                return;

            OSSignalEvent(&s_Event);
        }

        // # region Device

        bool Device::open()
        {
            AXInitWithParams(&AX_INIT_PARAMS);
            s_Init = AXIsInit();

            OSInitEvent(&s_Event, false, OS_EVENT_MODE_AUTO);
            AXRegisterAppFrameCallback(audioCallback);

            return s_Init;
        }

        void Device::update()
        {
            OSWaitEvent(&s_Event);
        }

        void Device::close()
        {
            if (!s_Init)
                return;

            AXDeregisterAppFrameCallback(audioCallback);
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
                if (voice)
                    AXVoiceBegin(voice);
            }

            UniqueVoiceScope(const UniqueVoiceScope&)            = delete;
            UniqueVoiceScope& operator=(const UniqueVoiceScope&) = delete;
            UniqueVoiceScope(UniqueVoiceScope&&)                 = delete;
            UniqueVoiceScope& operator=(UniqueVoiceScope&&)      = delete;

            ~UniqueVoiceScope()
            {
                if (this->voice)
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
            std::memset(this->buffer.data_pcm16, 0, size);

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
            AXVoiceOffsets offsets {};
            AXGetVoiceOffsets(this->buffer.voices[0], &offsets);
            const auto running = AXIsVoiceRunning(this->buffer.voices[0]);

            return offsets.currentOffset == offsets.endOffset || (!running && offsets.currentOffset == 0);
        }

        void Buffer::prepare(const void* data, size_t size, int samples, bool own)
        {
            if (data == nullptr || size == 0 || !this->buffer.data_pcm16)
                return;

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                int16_t* dst = this->buffer.data_pcm16 + (channel * samples);
                for (int i = 0; i < samples; i++)
                {
                    const auto sample = ((const int16_t*)data)[i * this->buffer.channels + channel];
                    dst[i]            = sample;
                }
                DCFlushRange(dst, samples * sizeof(int16_t));
            }

            AXVoiceOffsets offsets {};
            offsets.dataType       = AX_VOICE_FORMAT_LPCM16;
            offsets.loopingEnabled = AX_VOICE_LOOP_DISABLED;
            offsets.loopOffset     = 0;
            offsets.endOffset      = samples;
            offsets.currentOffset  = 0;

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                offsets.data = this->buffer.data_pcm16 + (channel * samples);

                UniqueVoiceScope scope(this->buffer.voices[channel]);
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
            const auto loop = looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);
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
                AXSetVoiceState(this->buffer.voices[channel], (AXVoiceState)status);
            }
        }

        void Buffer::setSampleRate(int samplerate)
        {
            const auto ratio = (float)samplerate / (float)AXGetInputSamplesPerSec();

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);
                AXSetVoiceSrcType(this->buffer.voices[channel], AX_VOICE_SRC_TYPE_LINEAR);
                AXSetVoiceSrcRatio(this->buffer.voices[channel], ratio);
            }

            this->buffer.samplerate = samplerate;
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

            AXVoiceDeviceMixData mix[0x06] {};

            for (int channel = 0; channel < this->buffer.channels; channel++)
            {
                UniqueVoiceScope scope(this->buffer.voices[channel]);
                AXSetVoiceVe(this->buffer.voices[channel], &data);

                mix[channel].bus[0].volume = ve;
                AXSetVoiceDeviceMix(this->buffer.voices[channel], AX_DEVICE_TYPE_DRC, 0, mix);
                AXSetVoiceDeviceMix(this->buffer.voices[channel], AX_DEVICE_TYPE_TV, 0, mix);
            }
        }

        float Buffer::getVolume() const
        {
            return this->buffer.voices[0]->volume / (float)0x8000;
        }

        // #endregion

        // #region Channel

        class BufferChannel
        {
          public:
            void reset(int samplerate, int channels, int bitdepth, float volume)
            {
                this->channels = channels;
                this->bitdepth = bitdepth;
                this->setSampleRate(samplerate);
                this->setVolume(volume);
            }

            float getVolume() const
            {
                if (this->buffer)
                    return this->buffer->getVolume();

                return this->volume;
            }

            void setVolume(float volume)
            {
                if (this->buffer)
                    this->buffer->setVolume(volume);

                this->volume = volume;
            }

            size_t getSampleOffset() const
            {
                if (this->buffer)
                    return this->buffer->getSampleCount();

                return 0;
            }

            void setSampleRate(int samplerate)
            {
                if (this->buffer)
                    this->buffer->setSampleRate(samplerate);

                this->samplerate = samplerate;
            }

            void setFormat(AX_VOICE_FORMAT format)
            {
                if (this->buffer)
                    this->buffer->setFormat(format);
            }

            void setStatus(AXVoiceState status)
            {
                if (this->buffer)
                    this->buffer->setStatus(status);
            }

            void setPaused(bool paused)
            {
                if (this->buffer)
                    this->buffer->setPaused(paused);

                this->paused = paused;
            }

            void addBuffer(Buffer* buffer)
            {
                this->buffer = buffer;

                buffer->setSampleRate(this->samplerate);
                buffer->setVolume(this->volume);
                buffer->setStatus(AX_VOICE_STATE_PLAYING);
                buffer->setPaused(false);
            }

            bool isPaused() const
            {
                if (this->buffer)
                    return this->buffer->isPaused();

                return this->paused;
            }

            void stop()
            {
                if (this->buffer)
                    this->buffer->setStatus(AX_VOICE_STATE_STOPPED);
            }

            bool isPlaying() const
            {
                return this->buffer && !this->buffer->isFinished() && !this->isPaused();
            }

          private:
            Buffer* buffer;
            int channels;
            int bitdepth;
            int samplerate;
            float volume;
            bool paused;
        };

        static std::array<BufferChannel, Channel::MAX_CHANNELS> s_Channels;

        static AX_VOICE_FORMAT getChannelFormat(AudioBase::AudioFormat format)
        {
            switch (format)
            {
                default:
                case AudioBase::FORMAT_MONO8:
                case AudioBase::FORMAT_STEREO8:
                    return AX_VOICE_FORMAT_LPCM8;
                case AudioBase::FORMAT_MONO16:
                case AudioBase::FORMAT_STEREO16:
                    return AX_VOICE_FORMAT_LPCM16;
            }
        }

        bool Channel::reset(size_t id, int channels, int bitdepth, int samplerate, float volume)
        {
            const auto format = dsp::Audio::getFormat(bitdepth, channels);

            if (format == AudioBase::FORMAT_NONE)
                return false;

            auto fmt = getChannelFormat(format);

            auto& channel = s_Channels[id];
            channel.reset(samplerate, channels, bitdepth, volume);
            channel.setFormat(fmt);

            return true;
        }

        void Channel::setVolume(size_t id, float volume)
        {
            auto& channel = s_Channels[id];
            channel.setVolume(volume);
        }

        float Channel::getVolume(size_t id)
        {
            auto& channel = s_Channels[id];
            return channel.getVolume();
        }

        size_t Channel::getSampleOffset(size_t id)
        {
            auto& channel = s_Channels[id];
            return channel.getSampleOffset();
        }

        bool Channel::addBuffer(size_t id, Buffer& buffer)
        {
            auto& channel = s_Channels[id];
            channel.addBuffer(&buffer);

            return true;
        }

        void Channel::pause(size_t id, bool paused)
        {
            auto& channel = s_Channels[id];
            channel.setPaused(paused);
            auto status = paused ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;
            channel.setStatus(status);
        }

        bool Channel::isPaused(size_t id)
        {
            auto& channel = s_Channels[id];
            return channel.isPaused();
        }

        bool Channel::isPlaying(size_t id)
        {
            auto& channel = s_Channels[id];
            return channel.isPlaying();
        }

        void Channel::stop(size_t id)
        {
            auto& channel = s_Channels[id];
            channel.stop();
        }
        // #endregion
    } // namespace audio
} // namespace love
