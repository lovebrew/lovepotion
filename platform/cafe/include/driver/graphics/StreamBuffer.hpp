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

            if (this->usage != BufferUsage::BUFFERUSAGE_VERTEX)
            {
                try
                {
                    this->data = new T[size];
                }
                catch (std::bad_alloc&)
                {
                    throw love::Exception("Failed to allocate StreamBuffer");
                }
            }
            else
            {
                this->buffer->elemCount = size;
                this->buffer->elemSize  = sizeof(T);
                this->buffer->flags     = USAGE_FLAGS;

                if (!GX2RCreateBuffer(this->buffer))
                    throw love::Exception("Failed to create StreamBuffer");

                GX2RSetAttributeBuffer(this->buffer, 0, sizeof(T), 0);
            }
        }

        MapInfo<T> map(size_t)
        {
            MapInfo<T> info {};

            if (this->usage == BufferUsage::BUFFERUSAGE_VERTEX)
                info.data = &((T*)GX2RLockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE))[this->index];
            else
                info.data = &this->data[this->index];

            info.size = this->bufferSize;
            return info;
        }

        size_t unmap(size_t)
        {
            if (this->usage == BufferUsage::BUFFERUSAGE_VERTEX)
                GX2RUnlockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);

            return this->index;
        }

        ~StreamBuffer()
        {
            if (this->usage == BufferUsage::BUFFERUSAGE_VERTEX)
                GX2RDestroyBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);

            delete this->buffer;
            delete[] this->data;
        }

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)this->data;
        }

      private:
        static constexpr auto USAGE_FLAGS = GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE |
                                            GX2R_RESOURCE_USAGE_GPU_READ | GX2R_RESOURCE_BIND_VERTEX_BUFFER;

        T* data;
        GX2RBuffer* buffer;
    };
} // namespace love
