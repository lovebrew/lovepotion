#include "common/Exception.hpp"

#include "modules/graphics/StreamBuffer.hpp"

#include <citro3d.h>

namespace love
{
    class StreamBufferClientMemory final : public StreamBufferBase
    {
      public:
        StreamBufferClientMemory(BufferUsage mode, size_t size) :
            StreamBufferBase(mode, size),
            data(nullptr),
            vbo(new C3D_BufInfo())
        {
            this->data = (uint8_t*)linearAlloc(size);

            if (this->data == nullptr)
                throw love::Exception("Failed to allocate memory for StreamBufferClientMemory");

            BufInfo_Init(this->vbo);

            int result = BufInfo_Add(this->vbo, this->data, sizeof(Vertex), 3, 0x210);

            if (result != 0)
                throw love::Exception("Failed to create StreamBufferClientMemory");
        }

        virtual ~StreamBufferClientMemory()
        {
            delete this->vbo;
            this->vbo = nullptr;

            linearFree(this->data);
        }

        size_t getGPUReadOffset() const override
        {
            return (size_t)this->data;
        }

        MapInfo map(size_t) override
        {
            return MapInfo(this->data, this->bufferSize);
        }

        size_t unmap(size_t) override
        {
            return (size_t)this->data;
        }

        void markUsed(size_t) override
        {}

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)this->vbo;
        }

      private:
        uint8_t* data;
        C3D_BufInfo* vbo;
    };

    StreamBufferBase* createStreamBuffer(BufferUsage usage, size_t size)
    {
        return new StreamBufferClientMemory(usage, size);
    }
} // namespace love
