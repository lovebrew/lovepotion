#include "common/Exception.hpp"

#include "modules/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <gx2/mem.h>
#include <gx2r/buffer.h>
#include <gx2r/draw.h>

namespace love
{
    static inline GX2RResourceFlags getBufferUsage(BufferUsage usage)
    {
        if (usage == BUFFERUSAGE_VERTEX)
            return GX2R_RESOURCE_BIND_VERTEX_BUFFER;

        return GX2R_RESOURCE_BIND_INDEX_BUFFER;
    }

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
            const auto flags    = getBufferUsage(this->mode);
            const auto elemSize = (this->mode == BUFFERUSAGE_VERTEX) ? sizeof(Vertex) : sizeof(uint16_t);

            this->buffer.elemCount = this->bufferSize;
            this->buffer.elemSize  = elemSize;
            this->buffer.flags     = flags | BUFFER_CREATE_FLAGS;

            return GX2RCreateBuffer(&this->buffer);
        }

        void unloadVolatile()
        {
            if (!GX2RBufferExists(&this->buffer))
                return;

            // GX2RDestroyBufferEx(&this->buffer, GX2R_RESOURCE_BIND_NONE);
        }

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

            if (this->mode == BufferUsage::BUFFERUSAGE_VERTEX)
                GX2RSetAttributeBuffer(&this->buffer, 0, this->buffer.elemSize, 0);

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

        static constexpr auto BUFFER_CREATE_FLAGS =
            GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
    };

    StreamBufferBase* createStreamBuffer(BufferUsage usage, size_t size)
    {
        return new StreamBuffer(usage, size);
    }
} // namespace love
