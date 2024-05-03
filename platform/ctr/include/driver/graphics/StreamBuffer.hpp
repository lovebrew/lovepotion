#pragma once

#include "driver/graphics/StreamBuffer.tcc"

#include <3ds.h>
#include <citro3d.h>

namespace love
{
    class StreamBuffer final : public StreamBufferBase
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase(usage, size), buffer(nullptr)
        {
            this->data = (uint8_t*)linearAlloc(size);

            if (this->data == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);

            if (usage != BUFFERUSAGE_VERTEX)
                return;

            this->buffer = new C3D_BufInfo();
            BufInfo_Init(this->buffer);
        }

        ~StreamBuffer()
        {
            linearFree(this->data);

            delete this->buffer;
            this->buffer = nullptr;
        }

        void bind(const void* offset)
        {
            if (this->buffer == nullptr)
                return;

            BufInfo_Add(this->buffer, offset, sizeof(Vertex), 3, 0x210);
            C3D_SetBufInfo(this->buffer);
        }

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)this->buffer;
        }

      private:
        C3D_BufInfo* buffer;
    };
} // namespace love
