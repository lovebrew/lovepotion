#include "common/Exception.hpp"

#include "modules/graphics/StreamBuffer.hpp"

#include <citro3d.h>

namespace love
{
    class StreamBufferClientMemory final : public StreamBufferBase
    {
      public:
        StreamBufferClientMemory(BufferUsage mode, size_t size) : StreamBufferBase(mode, size), data(nullptr)
        {
            this->data = (uint8_t*)linearAlloc(size);

            if (this->data == nullptr)
                throw love::Exception("Failed to allocate memory for StreamBufferClientMemory");
        }

        virtual ~StreamBufferClientMemory()
        {
            linearFree(this->data);
        }

        size_t getGPUReadOffset() const override
        {
            return (size_t)this->data;
        }

        MapInfo map(size_t) override
        {
            return MapInfo(this->data, this->bufferSize);
        }

        size_t unmap(size_t) override
        {
            return (size_t)this->data;
        }

        void markUsed(size_t) override
        {}

        ptrdiff_t getHandle() const override
        {
            return 0;
        }

      private:
        uint8_t* data;
    };

    StreamBufferBase* createStreamBuffer(BufferUsage usage, size_t size)
    {
        return new StreamBufferClientMemory(usage, size);
    }
} // namespace love
