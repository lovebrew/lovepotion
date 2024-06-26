#pragma once

#include "driver/display/deko.hpp"
#include "driver/graphics/StreamBuffer.tcc"

namespace love
{
    template<typename T>
    class StreamBuffer final : public StreamBufferBase<T>
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase<T>(usage, size)
        {
            size_t align = (size + DK_CMDMEM_ALIGNMENT - 1) & ~(DK_CMDMEM_ALIGNMENT - 1);

            this->memory = dk::MemBlockMaker { d3d.getDevice(), align }
                               .setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_CpuUncached)
                               .create();
        }

        ~StreamBuffer()
        {}

        MapInfo<T> StreamBuffer::map(size_t) override
        {
            MapInfo<T> info {};
            info.data = &this->memory.getCpuAddr()[this->index];
            info.size = this->bufferSize - this->frameGPUReadOffset;

            return info;
        }

        ptrdiff_t getHandle() const override
        {
            return 0;
        }

      private:
        dk::UniqueMemBlock memory;
        uint32_t sliceSize;
    };
} // namespace love
