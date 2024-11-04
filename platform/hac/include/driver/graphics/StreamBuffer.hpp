#pragma once

#include "driver/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include "driver/display/deko.hpp"
#include "driver/display/deko3d/CMemPool.h"

namespace love
{
    template<typename T>
    class StreamBuffer final : public StreamBufferBase<T>
    {
      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase<T>(usage, size)
        {
            this->sliceSize = (size + DK_CMDMEM_ALIGNMENT - 1) & ~(DK_CMDMEM_ALIGNMENT - 1);
        }

        StreamBuffer(const StreamBuffer&) = delete;

        StreamBuffer& operator=(const StreamBuffer&) = delete;

        ~StreamBuffer()
        {
            this->memory.destroy();
        }

        MapInfo<T> map(size_t)
        {
            MapInfo<T> info {};
            info.data = &((T*)this->memory.getCpuAddr())[this->index];
            info.size = this->bufferSize - this->frameGPUReadOffset;

            return info;
        }

        ptrdiff_t getHandle() const
        {
            return (ptrdiff_t)this->memory.getGpuAddr();
        }

      private:
        CMemPool::Handle memory;
        uint32_t sliceSize;
    };
} // namespace love
