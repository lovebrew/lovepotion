#include "modules/audio/Source.hpp"

#include <cstring>

#include <3ds.h>

namespace love
{
    StaticDataBuffer::StaticDataBuffer(const SoundData* data) :
        buffer {},
        _clone {},
        size(data->getSize()),
        nsamples(data->getSampleCount())
    {
        this->buffer.data_pcm16 = (int16_t*)linearAlloc(size);
        this->buffer.nsamples   = nsamples;

        if (this->buffer.data_pcm16 == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        std::memcpy(this->buffer.data_pcm16, data->getData(), size);
        DSP_FlushDataCache(this->buffer.data_pcm16, size);
    }

    void StaticDataBuffer::setLooping(bool looping)
    {
        this->buffer.looping = looping;
    }

    void* StaticDataBuffer::getBuffer() const
    {
        return this->buffer.data_pcm16;
    }

    AudioBuffer& StaticDataBuffer::clone(const size_t offsetSamples, const int channels)
    {
        this->_clone.data_pcm16 = (int16_t*)this->getBuffer() + offsetSamples;
        this->_clone.nsamples   = this->nsamples - (offsetSamples / channels);
        this->_clone.looping    = this->buffer.looping;
        this->_clone.status     = NDSP_WBUF_DONE;

        return this->_clone;
    }

    StaticDataBuffer::~StaticDataBuffer()
    {
        linearFree(this->buffer.data_pcm16);
    }
} // namespace love
