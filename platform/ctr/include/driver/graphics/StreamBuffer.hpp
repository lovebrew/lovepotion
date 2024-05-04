#pragma once

#include "driver/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <3ds.h>
#include <citro3d.h>

namespace love
{
    class StreamBuffer final : public StreamBufferBase, public Volatile
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase(usage, size), buffer(nullptr)
        {
            this->loadVolatile();
        }

        bool loadVolatile()
        {
            const auto size = this->getSize() * BUFFER_FRAMES;
            this->data      = (uint8_t*)linearAlloc(size);

            if (this->data == nullptr)
                return false;

            if (this->usage != BufferUsage::BUFFERUSAGE_VERTEX)
                return true;

            this->buffer = new C3D_BufInfo();
            BufInfo_Init(this->buffer);

            BufInfo_Add(this->buffer, this->data, sizeof(Vertex), 3, 0x210);
            C3D_SetBufInfo(this->buffer);

            return true;
        }

        void unloadVolatile()
        {
            linearFree(this->data);

            delete this->buffer;
            this->buffer = nullptr;
        }

        ~StreamBuffer()
        {
            this->unloadVolatile();
        }

        size_t unmap(size_t) override
        {
            return (size_t)this->data;
        }

        ptrdiff_t getHandle() const override
        {
            return 0;
        }

      private:
        C3D_BufInfo* buffer;
    };
} // namespace love
