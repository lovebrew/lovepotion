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

        ~StreamBuffer()
        {
            linearFree(this->data);
        }

        MapInfo<T> map(size_t) override
        {
            MapInfo<T> info {};
            info.data = &this->data[this->index];
            info.size = this->bufferSize - this->frameGPUReadOffset;

            return info;
        }

        size_t unmap(size_t) override
        {
            return this->index;
        }

        void markUsed(int count) override
        {
            this->index += count;
            this->frameGPUReadOffset += (count * sizeof(T));
        }

        void nextFrame() override
        {
            this->index              = 0;
            this->frameGPUReadOffset = 0;
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
