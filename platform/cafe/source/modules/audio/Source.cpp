#include "modules/audio/Source.hpp"

#include <cstring>

namespace love
{
    StaticDataBuffer::StaticDataBuffer(const SoundData* data) :
        buffer {},
        _clone {},
        data(nullptr),
        size(data->getSize()),
        nsamples(data->getSampleCount())
    {
        this->buffer.initialize(data->getChannelCount());

        this->buffer.data     = (int16_t*)malloc(this->size);
        this->buffer.nsamples = this->nsamples;

        if (!this->buffer.data)
            throw love::Exception(E_OUT_OF_MEMORY);

        std::memcpy(this->buffer.data, data->getData(), this->size);
        DCFlushRange(this->buffer.data, this->size);
    }

    void* StaticDataBuffer::getBuffer() const
    {
        return this->buffer.data;
    }

    void StaticDataBuffer::setLooping(bool looping)
    {
        this->buffer.setLooping(looping);
    }

    AudioBuffer& StaticDataBuffer::clone(const size_t offsetSamples, const int channels)
    {
        if (!this->buffer.isInitialized())
            throw love::Exception("Buffer is not initialized.");

        this->_clone.initialize(channels);
        this->_clone.data     = ((int16_t*)this->getBuffer()) + offsetSamples;
        this->_clone.nsamples = (this->buffer.getSampleCount() - (offsetSamples / channels));
        this->_clone.setLooping(this->buffer.isLooping());

        return this->_clone;
    }

    StaticDataBuffer::~StaticDataBuffer()
    {
        if (this->buffer.data)
            free(this->buffer.data);
    }
} // namespace love
