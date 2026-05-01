#include "common/Exception.hpp"

#include "driver/display/UniqueBuffer.hpp"
#include "modules/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <gx2/mem.h>
#include <gx2r/buffer.h>
#include <gx2r/draw.h>

namespace love
{
    class StreamBuffer final : public StreamBufferBase, public Volatile
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase(usage, size), buffer {}
        {
            this->loadVolatile();
        }

        ~StreamBuffer()
        {
            this->unloadVolatile();
        }

        bool loadVolatile()
        {
            GX2RResourceFlags flags;
            UniqueBuffer::getConstant(this->mode, flags);

            const auto elemSize = (this->mode == BUFFERUSAGE_VERTEX) ? sizeof(Vertex) : sizeof(uint16_t);

            this->buffer.elemCount = this->bufferSize;
            this->buffer.elemSize  = elemSize;
            this->buffer.flags     = flags;

            return GX2RCreateBuffer(&this->buffer);
        }

        void unloadVolatile()
        {}

        StreamBuffer(const StreamBuffer&) = delete;

        StreamBuffer& operator=(const StreamBuffer&) = delete;

        StreamBuffer(StreamBuffer&&) = delete;

        StreamBuffer& operator=(StreamBuffer&&) = delete;

        MapInfo map(size_t /* minSize */) override
        {
            MapInfo info {};

            auto* data = (uint8_t*)GX2RLockBufferEx(&this->buffer, GX2R_RESOURCE_BIND_NONE);

            info.data = &data[this->frameGPUReadOffset];
            info.size = this->bufferSize - this->frameGPUReadOffset;

            return info;
        }

        size_t unmap(size_t /* usedSize */) override
        {
            GX2RUnlockBufferEx(&this->buffer, GX2R_RESOURCE_BIND_NONE);
            return this->frameGPUReadOffset;
        }

        size_t getGPUReadOffset() const override
        {
            return this->frameGPUReadOffset;
        }

        void nextFrame() override
        {
            this->frameGPUReadOffset = 0;
        }

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)std::addressof(this->buffer);
        }

        size_t getSize() const override
        {
            return this->bufferSize;
        }

      private:
        GX2RBuffer buffer;
    };

    StreamBufferBase* createStreamBuffer(BufferUsage usage, size_t size)
    {
        return new StreamBuffer(usage, size);
    }
} // namespace love
