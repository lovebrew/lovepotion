#pragma once

#include "driver/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <gx2/mem.h>
#include <gx2r/buffer.h>
#include <gx2r/draw.h>

#include "utility/logfile.hpp"

namespace love
{
    template<typename T>
    class StreamBuffer final : public StreamBufferBase<T>
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase<T>(usage, size)
        {
            this->buffer = new GX2RBuffer();

            if (this->buffer == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);

            auto flags = (usage == BufferUsage::BUFFERUSAGE_VERTEX) ? GX2R_RESOURCE_BIND_VERTEX_BUFFER
                                                                    : GX2R_RESOURCE_BIND_INDEX_BUFFER;

            this->buffer->elemCount = size;
            this->buffer->elemSize  = sizeof(T);
            this->buffer->flags     = flags | BUFFER_CREATE_FLAGS;

            if (!GX2RCreateBuffer(this->buffer))
                throw love::Exception("Failed to create StreamBuffer");
        }

        MapInfo<T> map(size_t)
        {
            MapInfo<T> info {};

            auto* data = (T*)GX2RLockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);

            info.data = &data[this->index];
            info.size = this->bufferSize - this->frameGPUReadOffset;

            return info;
        }

        size_t unmap(size_t)
        {
            GX2RUnlockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);

            if (this->usage == BufferUsage::BUFFERUSAGE_VERTEX)
                GX2RSetAttributeBuffer(this->buffer, 0, this->buffer->elemSize, 0);

            return this->index;
        }

        ~StreamBuffer()
        {
            GX2RDestroyBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);
            delete this->buffer;
        }

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)this->buffer;
        }

      private:
        static constexpr auto BUFFER_CREATE_FLAGS =
            GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;

        GX2RBuffer* buffer;
    };
} // namespace love
