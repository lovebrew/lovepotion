#include "modules/graphics/Buffer.hpp"

#include "common/Exception.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/vertex.hpp"

#include "driver/display/deko.hpp"
#include <cstdlib>
#include <cstring>

#include "common/debug.hpp"

namespace love
{
    Buffer::Buffer(GraphicsBase* gfx, const Settings& settings, const BufferFormat& format, const void* data,
                   size_t size, size_t length) :
        BufferBase(gfx, settings, format, size, length),
        handle {}
    {
        size   = this->getSize();
        length = this->getArrayLength();

        if (this->usage & BUFFERUSAGEFLAG_VERTEX)
        {
            this->mapUsage     = BUFFERUSAGE_VERTEX;
            this->handle.state = { (uint32_t)this->arrayStride, 0 };
        }
        else if (this->usage & BUFFERUSAGEFLAG_INDEX)
            this->mapUsage = BUFFERUSAGE_INDEX;

        if (this->dataUsage == BUFFERDATAUSAGE_STREAM)
            this->ownsMemoryMap = true;

        std::vector<uint8_t> emptydata;
        if (settings.zeroInitialize && data == nullptr)
        {
            try
            {

                emptydata.resize(this->getSize());
                data = emptydata.data();
            }
            catch (std::exception&)
            {
                data = nullptr;
            }
        }

        if (!this->load(data))
        {
            this->unloadVolatile();
            throw love::Exception("Could not create Buffer with {:d} bytes (out of VRAM?)", size);
        }
    }

    Buffer::~Buffer()
    {
        this->unloadVolatile();
    }

    bool Buffer::loadVolatile()
    {
        if (this->bytes)
            return true;

        return this->load(nullptr);
    }

    void Buffer::unloadVolatile()
    {
        this->mapped = false;
        std::free(this->bytes);
        this->bytes = nullptr;
        std::free(this->staging);
        this->staging = nullptr;
        this->handle.memory.destroy();
    }

    bool Buffer::load(const void* data)
    {

        auto sliceSize = (((this->getSize() / 1) + DK_CMDMEM_ALIGNMENT - 1) & ~(DK_CMDMEM_ALIGNMENT - 1));
        auto& pool     = d3d.getMemoryPool(deko3d::MEMORYPOOL_DATA);
        this->handle.memory = pool.allocate(sliceSize, alignof(uint8_t));

        if (!this->handle.memory)
            return false;

        if (data != nullptr)
            std::memcpy(this->handle.memory.getCpuAddr(), data, this->getSize());
        else
            std::memset(this->handle.memory.getCpuAddr(), 0, this->getSize());

        return true;
    }

    bool Buffer::supportsOrphan() const
    {
        return this->dataUsage == BUFFERDATAUSAGE_STREAM || this->dataUsage == BUFFERDATAUSAGE_DYNAMIC;
    }

    void* Buffer::map(MapType map, size_t offset, size_t size)
    {
        if (size == 0)
            return nullptr;

        if (map == MAP_WRITE_INVALIDATE &&
            (this->isImmutable() || this->dataUsage == BUFFERDATAUSAGE_READBACK))
        {
            return nullptr;
        }

        if (map == MAP_READ_ONLY && this->dataUsage != BUFFERDATAUSAGE_READBACK)
            return nullptr;

        Range r(offset, size);

        if (!Range(0, this->getSize()).contains(r))
            return nullptr;

        this->mapped      = true;
        this->mappedType  = map;
        this->mappedRange = r;

        if (map == MAP_READ_ONLY)
            return (void*)((uint8_t*)this->handle.memory.getCpuAddr() + offset);

        try
        {
            this->staging = (uint8_t*)std::malloc(this->getSize());
        }
        catch (std::bad_alloc&)
        {
            this->mapped = false;
            return nullptr;
        }

        return (void*)this->staging;
    }

    void Buffer::unmap(size_t offset, size_t size)
    {
        Range r(offset, size);

        if (!this->mapped || !this->mappedRange.contains(r))
            return;

        this->mapped = false;

        if (this->mappedType == MAP_READ_ONLY)
            return;

        if (this->supportsOrphan() && this->mappedRange.first == 0 &&
            this->mappedRange.getSize() == this->getSize())
        {
            offset = 0;
            size   = this->getSize();
        }

        auto* data = this->staging + (offset - this->mappedRange.getOffset());
        this->fill(offset, size, data);
    }

    bool Buffer::fill(size_t offset, size_t size, const void* data)
    {
        if (size == 0 || this->isImmutable() || this->dataUsage == BUFFERDATAUSAGE_READBACK)
            return false;

        size_t bufferSize = this->getSize();

        if (!Range(0, bufferSize).contains(Range(offset, size)))
            return false;

        if (this->supportsOrphan() && size == bufferSize)
            std::memcpy(this->handle.memory.getCpuAddr(), data, bufferSize);
        else
            std::memcpy((uint8_t*)this->handle.memory.getCpuAddr() + offset, data, size);

        return true;
    }

    void Buffer::clearInternal(size_t offset, size_t size)
    {
        if (size == 0)
            return;

        try
        {
            std::vector<uint8_t> empty(size);
            this->fill(offset, size, empty.data());
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }
    }

    void Buffer::copyTo(BufferBase* destination, size_t sourceOffset, size_t destOffset, size_t size)
    {
        if (destination == nullptr || size == 0)
            return;

        const char* src = (const char*)this->handle.memory.getCpuAddr() + sourceOffset;
        destination->fill(destOffset, size, src);
    }
} // namespace love
