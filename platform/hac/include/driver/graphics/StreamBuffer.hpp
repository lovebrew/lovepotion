#pragma once

#include "driver/display/deko.hpp"

#include "driver/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

namespace love
{
    template<typename T>
    class StreamBuffer final : public StreamBufferBase<T>
    {
      public:
        StreamBuffer(BufferUsage mode, size_t size) : StreamBufferBase<T>(mode, size), memory {}, sliceSize(0)
        {
            this->sliceSize = (size + DK_CMDMEM_ALIGNMENT - 1) & ~(DK_CMDMEM_ALIGNMENT - 1);
        }

        bool allocate(CMemPool& pool)
        {
            this->memory = pool.allocate(this->sliceSize, alignof(T));
            return this->memory;
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

        size_t unmap(size_t);

        ptrdiff_t getHandle() const
        {
            return (ptrdiff_t)this->memory.getCpuAddr();
        }

      private:
        CMemPool::Handle memory;
        uint32_t sliceSize;
    };
} // namespace love
