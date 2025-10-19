#include "modules/graphics/StreamBuffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include "driver/display/deko.hpp"

namespace love
{
    class StreamBuffer final : public StreamBufferBase
    {
      private:
        static constexpr int SLICE_COUNT = MAX_RENDERTARGETS;

      public:
        StreamBuffer(BufferUsage usage, size_t size) : StreamBufferBase(usage, size), handle {}, sliceSize(0)
        {
            this->sliceSize = ((size / SLICE_COUNT) + DK_CMDMEM_ALIGNMENT - 1) & ~(DK_CMDMEM_ALIGNMENT - 1);

            auto& pool   = d3d.getMemoryPool(deko3d::MEMORYPOOL_DATA);
            this->handle = pool.allocate(SLICE_COUNT * this->sliceSize, alignof(uint8_t));

            if (!this->handle)
                throw love::Exception(E_OUT_OF_MEMORY);
        }

        StreamBuffer(const StreamBuffer&) = delete;

        StreamBuffer& operator=(const StreamBuffer&) = delete;

        ~StreamBuffer()
        {
            this->handle.destroy();
        }

        size_t getSize() const override
        {
            return this->sliceSize * SLICE_COUNT;
        }

        MapInfo map(size_t /* minSize */) override
        {
            MapInfo info {};

            if (!this->handle)
                return info;

            const size_t offset = (this->frameIndex * this->sliceSize) + this->frameGPUReadOffset;

            if (offset >= this->handle.getSize())
                return info;

            auto* cpuAddr = (uint8_t*)this->handle.getCpuAddr();

            info.data = cpuAddr + offset;
            info.size = this->sliceSize - this->frameGPUReadOffset;

            return info;
        }

        size_t unmap(size_t /* usedSize */) override
        {
            if (!this->handle)
                return 0;

            const auto offset = (this->frameIndex * this->sliceSize);
            d3d.bindBuffer(this->mode, this->handle.getGpuAddr() + offset, this->sliceSize);

            return this->frameGPUReadOffset;
        }

        size_t getGPUReadOffset() const override
        {
            return (this->frameIndex * this->sliceSize) + this->frameGPUReadOffset;
        }

        void nextFrame() override
        {
            this->frameIndex         = (this->frameIndex + 1) % SLICE_COUNT;
            this->frameGPUReadOffset = 0;
        }

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)this->handle.getCpuAddr();
        }

      private:
        CMemPool::Handle handle;
        uint32_t sliceSize;
    };

    StreamBufferBase* createStreamBuffer(BufferUsage usage, size_t size)
    {
        return new StreamBuffer(usage, size);
    }
} // namespace love
