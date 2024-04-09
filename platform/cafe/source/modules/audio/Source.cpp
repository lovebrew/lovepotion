#include "modules/audio/Source.hpp"

#include <coreinit/memorymap.h>

#include <cstring>

namespace love
{
    StaticDataBuffer::StaticDataBuffer(const void* data, size_t size, size_t nsamples) :
        size(size),
        nsamples(nsamples)
    {
        this->buffer = (int16_t*)malloc(size);

        if (this->buffer == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        std::memcpy(this->buffer, data, size);
    }

    StaticDataBuffer::~StaticDataBuffer()
    {
        if (this->buffer)
            free(this->buffer);
    }
} // namespace love
