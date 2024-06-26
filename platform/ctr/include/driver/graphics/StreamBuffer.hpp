#pragma once

#include "driver/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <3ds.h>
#include <citro3d.h>

namespace love
{
    template<typename T>
    class StreamBuffer final : public StreamBufferBase<T>
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase<T>(usage, size), buffer {}
        {
            this->data = (T*)linearAlloc(this->bufferSize);

            if (this->data == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);

            if (usage != BUFFERUSAGE_VERTEX)
                return;

            BufInfo_Init(&this->buffer);
            BufInfo_Add(&this->buffer, this->data, sizeof(Vertex), 3, 0x210);

            C3D_SetBufInfo(&this->buffer);
        }

        MapInfo<T> map(size_t)
        {
            MapInfo<T> info {};
            info.data = &this->data[this->index];
            info.size = this->bufferSize - this->frameGPUReadOffset;

            return info;
        }

        ~StreamBuffer()
        {
            linearFree(this->data);
        }

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)this->data;
        }

      private:
        T* data;
        C3D_BufInfo buffer;
    };
} // namespace love
