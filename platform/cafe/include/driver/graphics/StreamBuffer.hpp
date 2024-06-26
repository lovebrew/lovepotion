#pragma once

#include "driver/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <gx2r/buffer.h>
#include <gx2r/draw.h>

namespace love
{
    template<typename T>
    class StreamBuffer final : public StreamBufferBase<T>
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) :
            StreamBufferBase<T>(usage, size),
            buffer(new GX2RBuffer())
        {
            this->buffer->elemCount = size / sizeof(T);
            this->buffer->elemSize  = sizeof(T);
            this->buffer->flags     = getBufferFlags(usage);

            if (!GX2RCreateBuffer(this->buffer))
                throw love::Exception("Failed to create StreamBuffer");

            if (usage == BufferUsage::BUFFERUSAGE_VERTEX)
                GX2RSetAttributeBuffer(this->buffer, 0, sizeof(T), 0);
        }

        MapInfo<T> map(size_t)
        {
            MapInfo<T> info {};

            info.data = &((T*)GX2RLockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE))[this->index];
            info.size = this->bufferSize - this->frameGPUReadOffset;

            return info;
        }

        size_t unmap(size_t)
        {
            GX2RUnlockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);
            return this->index;
        }

        void debug(std::function<void(T*)> func)
        {
            T* data = (T*)GX2RLockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);
            func(data);
            GX2RUnlockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);
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
        static constexpr auto BASE_FLAGS =
            GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;

        static GX2RResourceFlags getBufferFlags(BufferUsage usage)
        {
            switch (usage)
            {
                case BufferUsage::BUFFERUSAGE_VERTEX:
                    return BASE_FLAGS | GX2R_RESOURCE_BIND_VERTEX_BUFFER;
                case BufferUsage::BUFFERUSAGE_INDEX:
                    return BASE_FLAGS | GX2R_RESOURCE_BIND_INDEX_BUFFER;
                default:
                    throw love::Exception("Invalid buffer usage");
            }
        }

        GX2RBuffer* buffer;
    };
} // namespace love
