#include "common/Exception.hpp"

#include "modules/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <3ds.h>
#include <citro3d.h>

namespace love
{
    class StreamBuffer final : public StreamBufferBase, public Volatile
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase(usage, size), data(nullptr), buffer {}
        {
            this->loadVolatile();
        }

        ~StreamBuffer()
        {
            this->unloadVolatile();
        }

        bool loadVolatile()
        {
            this->data = (uint8_t*)linearAlloc(this->bufferSize);

            if (this->data == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);

            if (this->mode != BUFFERUSAGE_VERTEX)
                return true;

            BufInfo_Init(&this->buffer);
            BufInfo_Add(&this->buffer, this->data, sizeof(Vertex), 3, 0x210);

            C3D_SetBufInfo(&this->buffer);
            return true;
        }

        void unloadVolatile()
        {
            linearFree(this->data);
        }

        MapInfo map(size_t /* minSize */) override
        {
            MapInfo info {};
            info.data = &this->data[this->frameGPUReadOffset];
            info.size = this->bufferSize - this->frameGPUReadOffset;
            return info;
        }

        size_t unmap(size_t /* usedSize */) override
        {
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
            return (ptrdiff_t)this->data;
        }

        size_t getSize() const override
        {
            return this->bufferSize;
        }

      private:
        uint8_t* data;
        C3D_BufInfo buffer;
    };

    StreamBufferBase* createStreamBuffer(BufferUsage usage, size_t size)
    {
        return new StreamBuffer(usage, size);
    }
} // namespace love
