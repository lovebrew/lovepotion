#include "modules/audio/Source.hpp"
#include "driver/DigitalSoundMem.hpp"

namespace love
{
    StaticDataBuffer::StaticDataBuffer(const void* data, size_t size, size_t nsamples) :
        buffer(nullptr),
        size(size),
        nsamples(nsamples)
    {
        this->buffer = (int16_t*)DigitalSoundMemory::getInstance().align(size, this->alignSize);

        if (this->buffer == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        std::copy_n((int16_t*)data, size, this->buffer);
    }

    StaticDataBuffer::~StaticDataBuffer()
    {
        if (this->buffer != nullptr)
            DigitalSoundMemory::getInstance().free(this->buffer, this->alignSize);
    }
} // namespace love
