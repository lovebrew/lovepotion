#include "modules/audio/Source.hpp"

#include <cstring>

namespace love
{
    StaticDataBuffer::StaticDataBuffer(const SoundData* data) :
        buffer {},
        _clone {},
        size(data->getSize()),
        nsamples(data->getSampleCount())
    {
        const int channels = data->getChannelCount();

        this->buffer.nsamples = this->nsamples;

        for (int channel = 0; channel < channels; channel++)
        {
            this->buffer.data_pcm16[channel] = (int16_t*)malloc(this->size);

            if (this->buffer.data_pcm16[channel] == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);
        }

        switch (channels)
        {
            case 1:
                std::memcpy(this->buffer.data_pcm16[0], data->getData(), this->size);
                DCFlushRange(this->buffer.data_pcm16[0], this->size);

                break;
            case 2:
            {
                /* separate stereo audio into two buffers */
                const int16_t* pcm16 = (int16_t*)data->getData();

                for (size_t index = 0; index < this->size; index += 2)
                {
                    this->buffer.data_pcm16[0][index / 2] = pcm16[index];
                    this->buffer.data_pcm16[1][index / 2] = pcm16[index + 1];
                }

                DCFlushRange(this->buffer.data_pcm16[0], this->size);
                DCFlushRange(this->buffer.data_pcm16[1], this->size);

                break;
            }
            default:
                throw love::Exception("Unsupported channel count");
        }
    }

    void* StaticDataBuffer::getBuffer() const
    {
        return nullptr;
    }

    void StaticDataBuffer::setLooping(bool looping)
    {
        this->buffer.setLooping(looping);
    }

    AudioBuffer& StaticDataBuffer::clone(const size_t offsetSamples, const int channels)
    {
        this->_clone.initialize(channels);

        for (int channel = 0; channel < channels; channel++)
            this->_clone.data_pcm16[channel] = this->buffer.data_pcm16[channel] + offsetSamples;

        this->_clone.nsamples = (this->buffer.getSampleCount() - (offsetSamples / channels));
        this->_clone.setLooping(this->buffer.isLooping());

        return this->_clone;
    }

    StaticDataBuffer::~StaticDataBuffer()
    {
        if (this->buffer.data_pcm16[0])
            free(this->buffer.data_pcm16[0]);

        if (this->buffer.data_pcm16[1])
            free(this->buffer.data_pcm16[1]);
    }
} // namespace love
