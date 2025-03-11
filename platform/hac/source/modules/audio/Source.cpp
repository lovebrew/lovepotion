#include "modules/audio/Source.hpp"
#include "driver/audio/DigitalSound.hpp"

#include <cstring>

namespace love
{
    StaticDataBuffer::StaticDataBuffer(const SoundData* data) :
        buffer {},
        _clone {},
        size(data->getSize()),
        nsamples(data->getSampleCount())
    {
        this->buffer.data_pcm16 = (int16_t*)DigitalSound::getInstance().allocateBuffer(size);

        if (this->buffer.data_pcm16 == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        std::memcpy(this->buffer.data_pcm16, data->getData(), size);
        armDCacheFlush(this->buffer.data_pcm16, size);
    }

    void* StaticDataBuffer::getBuffer() const
    {
        return this->buffer.data_pcm16;
    }

    void StaticDataBuffer::setLooping(bool looping)
    {
        this->buffer.is_looping = looping;
    }

    AudioBuffer& StaticDataBuffer::clone(const size_t offsetSamples, const int channels)
    {
        this->_clone.data_pcm16        = (int16_t*)this->getBuffer() + offsetSamples;
        this->_clone.end_sample_offset = this->getSampleCount() - (offsetSamples / channels);
        this->_clone.is_looping        = this->buffer.is_looping;

        return this->_clone;
    }

    StaticDataBuffer::~StaticDataBuffer()
    {
        DigitalSound::getInstance().freeBuffer(this->buffer.data_pcm16);
    }
} // namespace love
