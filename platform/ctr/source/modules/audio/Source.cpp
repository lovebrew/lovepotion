#include "modules/audio/Source.hpp"

#include <cstring>

#include <3ds.h>

namespace love
{
    StaticDataBuffer::StaticDataBuffer(const void* data, size_t size, size_t nsamples) :
        size(size),
        nsamples(nsamples)
    {
        this->buffer = (int16_t*)linearAlloc(size);

        if (this->buffer == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        std::memcpy(this->buffer, (int16_t*)data, size);
    }

    StaticDataBuffer::~StaticDataBuffer()
    {
        linearFree(this->buffer);
    }
} // namespace love
