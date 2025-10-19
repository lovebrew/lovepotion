#include "common/Exception.hpp"
#include "common/Result.hpp"

#include "driver/DigitalSound.tcc"
#include "modules/audio/dsp/Audio.hpp"

#include <cstring>

namespace love
{
    namespace audio
    {
        class InvalidFormatException : public love::Exception
        {
          public:
            InvalidFormatException(int channels, int bitdepth) :
                Exception("%d-channel Sources with %d bits per sample are not supported.", channels, bitdepth)
            {}
        };

        LightEvent s_Event;
        volatile bool s_Init = false;

        // #region Device

        static void audioCallback(void*)
        {
            if (!s_Init)
                return;

            LightEvent_Signal(&s_Event);
        }

        bool Device::open()
        {
            if (!ResultCode(ndspInit()))
                return false;

            LightEvent_Init(&s_Event, RESET_ONESHOT);
            ndspSetCallback(audioCallback, nullptr);

            for (size_t index = 0; index < Channel::MAX_CHANNELS; index++)
                ndspChnReset(index);

            s_Init = true;
            return s_Init;
        }

        void Device::update()
        {
            LightEvent_Wait(&s_Event);
        }

        void Device::close()
        {
            if (!s_Init)
                return;

            s_Init = false;

            ndspSetCallback(nullptr, nullptr);
            ndspExit();
        }

        void Device::setMasterVolume(float volume)
        {
            ndspSetMasterVol(volume);
        }

        float Device::getMasterVolume()
        {
            return ndspGetMasterVol();
        }
        // #endregion

        // #region Buffer

        Buffer::Buffer(const size_t size, int) : buffer {}
        {
            if (size == 0)
                throw love::Exception("Audio buffer size cannot be zero.");

            this->buffer.data_pcm16 = (int16_t*)linearAlloc(size);

            if (!this->buffer.data_pcm16)
                throw love::Exception(E_OUT_OF_MEMORY);
        }

        void Buffer::destroy()
        {
            linearFree(this->buffer.data_pcm16);
        }

        bool Buffer::isFinished() const
        {
            return this->buffer.status == NDSP_WBUF_DONE;
        }

        void Buffer::prepare(const void* data, const size_t size, int samples, bool own)
        {
            if (data == nullptr || size == 0)
                return;

            this->buffer.nsamples = samples;

            if (own)
            {
                if (!this->buffer.data_pcm16)
                    return;

                std::memcpy(this->buffer.data_pcm16, data, size);
            }
            else
                this->buffer.data_pcm16 = (int16_t*)data;

            DSP_FlushDataCache(this->buffer.data_pcm16, size);
        }

        size_t Buffer::getSampleCount() const
        {
            return this->buffer.nsamples;
        }

        void Buffer::setLooping(bool looping)
        {
            this->buffer.looping = looping;
        }

        bool Buffer::isLooping() const
        {
            return this->buffer.looping;
        }

        void Buffer::setStatus(uint8_t status)
        {
            this->buffer.status = status;
        }

        // #endregion

        // #region Channel

        static NdspFormat getChannelFormat(AudioBase::AudioFormat format)
        {
            switch (format)
            {
                default:
                case AudioBase::FORMAT_MONO8:
                    return NDSP_FORMAT_MONO_PCM8;
                case AudioBase::FORMAT_MONO16:
                    return NDSP_FORMAT_MONO_PCM16;
                case AudioBase::FORMAT_STEREO8:
                    return NDSP_FORMAT_STEREO_PCM8;
                case AudioBase::FORMAT_STEREO16:
                    return NDSP_FORMAT_STEREO_PCM16;
            }
        }

        bool Channel::reset(size_t id, int channels, int bitdepth, int samplerate, float volume)
        {
            const auto format = dsp::Audio::getFormat(bitdepth, channels);

            if (format == AudioBase::FORMAT_NONE)
                throw InvalidFormatException(channels, bitdepth);

            NdspFormat fmt = getChannelFormat(format);

            ndspChnReset(id);
            ndspChnSetFormat(id, fmt);
            ndspChnSetRate(id, samplerate);
            ndspChnSetInterp(id, NDSP_INTERP_POLYPHASE);
            Channel::setVolume(id, volume);

            return true;
        }

        void Channel::setVolume(size_t id, float volume)
        {
            std::array<float, 12> mix { volume, volume };
            ndspChnSetMix(id, mix.data());
        }

        float Channel::getVolume(size_t id)
        {
            std::array<float, 12> mix {};
            ndspChnGetMix(id, mix.data());

            return mix.at(0);
        }

        size_t Channel::getSampleOffset(size_t id)
        {
            return ndspChnGetSamplePos(id);
        }

        bool Channel::addBuffer(size_t id, Buffer& buffer)
        {
            ndspChnWaveBufAdd(id, buffer.getHandle());
            return true;
        }

        void Channel::pause(size_t id, bool paused)
        {
            ndspChnSetPaused(id, paused);
        }

        bool Channel::isPaused(size_t id)
        {
            return ndspChnIsPaused(id);
        }

        bool Channel::isPlaying(size_t id)
        {
            return ndspChnIsPlaying(id);
        }

        void Channel::stop(size_t id)
        {
            ndspChnWaveBufClear(id);
        }
        // #endregion
    } // namespace audio
} // namespace love
