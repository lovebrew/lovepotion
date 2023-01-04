#pragma once

#include <utilities/driver/vertex_ext.hpp>

#include <gx2r/buffer.h>

namespace love
{
    using namespace vertex;

    class DrawBuffer
    {
        class LockedBuffer
        {
          private:
            LockedBuffer(GX2RBuffer* buffer) :
                buffer(buffer),
                data((Vertex*)GX2RLockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE))
            {}

          public:
            friend class DrawBuffer;

            Vertex& operator*()
            {
                return this->data[0];
            }

            Vertex& operator[](size_t index)
            {
                return this->data[index];
            }

            Vertex* operator->()
            {
                return this->data;
            }

            ~LockedBuffer()
            {
                GX2RUnlockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);
            }

          private:
            Vertex* data;
            GX2RBuffer* buffer;
        };

      public:
        DrawBuffer(int elements)
        {
            this->buffer.elemCount = elements;
            this->buffer.elemSize  = VERTEX_SIZE;
            this->buffer.flags     = DrawBuffer::CREATE_FLAGS;

            this->valid = GX2RCreateBuffer(&this->buffer);
        };

        DrawBuffer(DrawBuffer&&) = delete;

        DrawBuffer(const DrawBuffer&) = delete;

        DrawBuffer& operator=(const DrawBuffer&) = delete;

        ~DrawBuffer()
        {
            if (!this->valid)
                return;

            GX2RDestroyBufferEx(&this->buffer, GX2R_RESOURCE_BIND_NONE);
        }

        LockedBuffer Lock()
        {
            if (!this->valid)
                return LockedBuffer(nullptr);

            return LockedBuffer(&this->buffer);
        }

        GX2RBuffer* GetBuffer()
        {
            return &this->buffer;
        }

        bool IsValid() const
        {
            return this->valid;
        }

      private:
        static constexpr auto CREATE_FLAGS =
            GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ |
            GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;

        GX2RBuffer buffer;
        bool valid;
    };
} // namespace love
