#pragma once

#include "driver/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <gx2r/buffer.h>
#include <gx2r/draw.h>

#include "utility/logfile.hpp"

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
            this->buffer->elemCount = size;
            this->buffer->elemSize  = sizeof(T);
            this->buffer->flags     = USAGE_FLAGS | getUsageFlags(usage);

            if (!GX2RCreateBuffer(this->buffer))
                throw love::Exception("Failed to create StreamBuffer");
        }

        MapInfo<T> map(size_t)
        {
            MapInfo<T> info {};

            auto* buffer = (T*)GX2RLockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);

            info.data = &buffer[this->index];
            info.size = this->bufferSize - this->frameGPUReadOffset;

            return info;
        }

        size_t unmap(size_t)
        {
            LOG("Unmapping StreamBuffer")
            GX2RUnlockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);

            if (this->usage == BufferUsage::BUFFERUSAGE_VERTEX)
                GX2RSetAttributeBuffer(this->buffer, 0, sizeof(T), 0);

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
        static constexpr auto USAGE_FLAGS =
            GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;

        static GX2RResourceFlags getUsageFlags(BufferUsage usage)
        {
            switch (usage)
            {
                case BufferUsage::BUFFERUSAGE_VERTEX:
                    return GX2R_RESOURCE_BIND_VERTEX_BUFFER;
                case BufferUsage::BUFFERUSAGE_INDEX:
                    return GX2R_RESOURCE_BIND_INDEX_BUFFER;
                default:
                    throw love::Exception("Invalid buffer usage");
            }
        }

        GX2RBuffer* buffer;
    };
} // namespace love
