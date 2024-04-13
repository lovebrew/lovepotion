#include "modules/audio/Source.hpp"

#include <cstring>

#include <3ds.h>

namespace love
{
    StaticDataBuffer::StaticDataBuffer(const SoundData* data) :
        buffer {},
        _clone {},
        size(data->getSize()),
        nsamples(data->getSampleCount()),
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
        this->bufferClone.data_pcm16 = (int16_t*)this->getBuffer() + offsetSamples;
        this->bufferClone.nsamples   = this->nsamples - (offsetSamples / channels);
        this->bufferClone.looping    = this->buffer.looping;
        this->bufferClone.status     = NDSP_WBUF_DONE;

        return bufferClone;
    }

    StaticDataBuffer::~StaticDataBuffer()
    {
        linearFree(this->buffer.data_pcm16);
    }
} // namespace love
