#include "common/Exception.hpp"
#include "common/Result.hpp"

#include "modules/audio/dsp/Audio.hpp"

#include "driver/DigitalSound.tcc"
#include "driver/audio/DigitalSoundMemory.hpp"

#include <cstring>

namespace love
{
    namespace audio
    {
        static constexpr AudioRendererConfig AUDIO_CONFIG = {
            .output_rate     = AudioRendererOutputRate_48kHz,
            .num_voices      = 24,
            .num_effects     = 0,
            .num_sinks       = 1,
            .num_mix_objs    = 1,
            .num_mix_buffers = 2,
        };

        static constexpr std::array<uint8_t, 2> SINK_CHANNELS = { 0, 1 };

        static AudioDriver s_Driver;
        static DigitalSoundMemory s_Memory;
        volatile bool s_Init = false;

        static std::array<bool, AUDIO_CONFIG.num_voices> s_VoiceInitialized;

        // #region Device

        bool Device::open()
        {
            if (!ResultCode(audrenInitialize(&AUDIO_CONFIG)))
                return false;

            if (!ResultCode(audrvCreate(&s_Driver, &AUDIO_CONFIG, 2)))
                return false;

            if (!s_Memory.initialize(&s_Driver))
                return false;

            if (audrvDeviceSinkAdd(&s_Driver, AUDREN_DEFAULT_DEVICE_NAME, 2, SINK_CHANNELS.data()) < 0)
                return false;

            if (!ResultCode(audrvUpdate(&s_Driver)))
                return false;

            if (!ResultCode(audrenStartAudioRenderer()))
                return false;

            s_Init = true;
            return s_Init;
        }

        void Device::update()
        {
            if (!s_Init)
                return;

            {
                std::unique_lock lock(Device::getMutex());
                audrvUpdate(&s_Driver);
            }
            audrenWaitFrame();
        }

        void Device::close()
        {
            if (!s_Init)
                return;

            audrvClose(&s_Driver);
            audrenExit();
            s_Init = false;
        }

        void Device::setMasterVolume(float volume)
        {
            std::unique_lock lock(Device::getMutex());

            for (auto mix : SINK_CHANNELS)
                audrvMixSetVolume(&s_Driver, mix, volume);
        }

        float Device::getMasterVolume()
        {
            std::unique_lock lock(Device::getMutex());
            return s_Driver.in_mixes[0].volume;
        }

        // #endregion

        // #region Buffer

        Buffer::Buffer(size_t size, int) : buffer {}
        {
            if (size == 0)
                throw love::Exception("Audio buffer size cannot be zero.");

            this->buffer.data_pcm16 = (int16_t*)s_Memory.allocate(size);

            if (!this->buffer.data_pcm16)
                throw love::Exception(E_OUT_OF_MEMORY);

            this->buffer.state = AudioDriverWaveBufState_Done;
        }

        void Buffer::destroy()
        {
            s_Memory.free(this->buffer.data_pcm16);
            this->buffer.data_pcm16 = nullptr;
        }

        bool Buffer::isFinished() const
        {
            return this->buffer.state == AudioDriverWaveBufState_Done;
        }

        void Buffer::prepare(const void* data, const size_t size, int samples, bool own)
        {
            if (data == nullptr || size == 0)
                return;

            this->buffer.size              = size;
            this->buffer.end_sample_offset = samples;

            if (own)
            {
                if (!this->buffer.data_pcm16)
                    return;

                std::memcpy(this->buffer.data_pcm16, data, size);
            }
            else
                this->buffer.data_pcm16 = (int16_t*)data;

            armDCacheFlush(this->buffer.data_pcm16, size);
        }

        size_t Buffer::getSampleCount() const
        {
            return this->buffer.end_sample_offset;
        }

        void Buffer::setLooping(bool looping)
        {
            this->buffer.is_looping = looping;
        }

        bool Buffer::isLooping() const
        {
            return this->buffer.is_looping;
        }

        void Buffer::setStatus(uint8_t status)
        {
            this->buffer.state = (AudioDriverWaveBufState)status;
        }

        // #endregion

        // #region Channel

        static PcmFormat getChannelFormat(AudioBase::AudioFormat format)
        {
            switch (format)
            {
                default:
                case AudioBase::FORMAT_MONO8:
                case AudioBase::FORMAT_STEREO8:
                    return PcmFormat_Int8;
                case AudioBase::FORMAT_MONO16:
                case AudioBase::FORMAT_STEREO16:
                    return PcmFormat_Int16;
            }
        }

        bool Channel::reset(size_t id, int channels, int bitdepth, int samplerate, float volume)
        {
            const auto format = dsp::Audio::getFormat(bitdepth, channels);

            if (format == AudioBase::FORMAT_NONE)
                return false;

            const auto fmt = getChannelFormat(format);

            std::unique_lock lock(Device::getMutex());
            const bool ok = audrvVoiceInit(&s_Driver, id, channels, fmt, samplerate);

            if (ok)
            {
                s_VoiceInitialized[id] = true;
                audrvVoiceSetDestinationMix(&s_Driver, id, AUDREN_FINAL_MIX_ID);
                audrvVoiceSetMixFactor(&s_Driver, id, 1.0f, 0, 0);

                if (channels == 2)
                    audrvVoiceSetMixFactor(&s_Driver, id, 1.0f, 0, 1);

                Channel::setVolume(id, volume);
            }
            else
                s_VoiceInitialized[id] = false;

            return ok;
        }

        void Channel::setVolume(size_t id, float volume)
        {
            std::unique_lock lock(Device::getMutex());
            audrvVoiceSetVolume(&s_Driver, id, volume);
        }

        float Channel::getVolume(size_t id)
        {
            std::unique_lock lock(Device::getMutex());
            return s_Driver.in_voices[id].volume;
        }

        size_t Channel::getSampleOffset(size_t id)
        {
            std::unique_lock lock(Device::getMutex());
            return audrvVoiceGetPlayedSampleCount(&s_Driver, id);
        }

        bool Channel::addBuffer(size_t id, Buffer& buffer)
        {
            if (id >= s_VoiceInitialized.size() || !s_VoiceInitialized[id])
                return false;

            std::unique_lock lock(Device::getMutex());
            bool success = audrvVoiceAddWaveBuf(&s_Driver, id, buffer.getHandle());

            if (!success)
                return false;

            audrvVoiceStart(&s_Driver, id);
            return true;
        }

        void Channel::pause(size_t id, bool paused)
        {
            std::unique_lock lock(Device::getMutex());
            audrvVoiceSetPaused(&s_Driver, id, paused);
        }

        bool Channel::isPaused(size_t id)
        {
            std::unique_lock lock(Device::getMutex());
            return audrvVoiceIsPaused(&s_Driver, id);
        }

        bool Channel::isPlaying(size_t id)
        {
            std::unique_lock lock(Device::getMutex());
            return audrvVoiceIsPlaying(&s_Driver, id);
        }

        void Channel::stop(size_t id)
        {
            std::unique_lock lock(Device::getMutex());
            audrvVoiceStop(&s_Driver, id);
        }

        // #endregion
    } // namespace audio
} // namespace love
