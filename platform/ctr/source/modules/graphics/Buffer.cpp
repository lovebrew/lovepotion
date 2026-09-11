#include "modules/graphics/Buffer.hpp"

#include "common/Exception.hpp"
#include "common/Module.hpp"
#include "driver/display/citro3d.hpp"
#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/vertex.hpp"

#include <3ds/allocator/linear.h>
#include <c3d/buffers.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace love
{
    Buffer::Buffer(GraphicsBase* gfx, const Settings& settings, const BufferFormat& format, const void* data,
                   size_t size, size_t length) :
        BufferBase(gfx, settings, format, size, length),
        buffer {}
    {
        size   = this->getSize();
        length = this->getArrayLength();

        if (this->usage & BUFFERUSAGEFLAG_VERTEX)
            this->mapUsage = BUFFERUSAGE_VERTEX;
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
        if (this->memoryMap != nullptr && this->ownsMemoryMap)
            std::free(this->memoryMap);
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
    }

    bool Buffer::load(const void* data)
    {
        if (this->usage & BUFFERUSAGEFLAG_VERTEX)
            BufInfo_Init(&this->buffer);

        this->bytes = (uint8_t*)linearAlloc(this->getSize());

        if (this->mapUsage == BUFFERUSAGE_VERTEX)
        {
            if (BufInfo_Add(&this->buffer, this->bytes, this->arrayStride, 3, 0x210) < 0)
                return false;
        }

        if (data != nullptr)
            std::memcpy(this->bytes, data, this->getSize());

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

        uint8_t* data = nullptr;

        if (map == MAP_READ_ONLY)
            return (void*)(this->memoryMap + offset);
        else if (this->ownsMemoryMap)
        {
            if (this->memoryMap == nullptr)
                this->memoryMap = (uint8_t*)std::malloc(this->getSize());
            data = this->memoryMap;
        }
        else
        {
            auto gfx = Module::getInstance<Graphics>(Module::M_GRAPHICS);
            data     = (uint8_t*)gfx->getBufferMapMemory(size);
        }

        if (data != nullptr)
        {
            this->mapped      = true;
            this->mappedType  = map;
            this->mappedRange = r;

            if (!this->ownsMemoryMap)
                this->memoryMap = data;
        }

        return data;
    }

    void Buffer::unmap(size_t offset, size_t size)
    {
        Range r(offset, size);

        if (!this->mapped || !this->mappedRange.contains(r))
            return;

        this->mapped = false;

        if (this->mappedType == MAP_READ_ONLY)
        {
            if (!this->ownsMemoryMap)
                this->memoryMap = nullptr;

            return;
        }

        if (this->supportsOrphan() && this->mappedRange.first == 0 &&
            this->mappedRange.getSize() == this->getSize())
        {
            offset = 0;
            size   = this->getSize();
        }

        auto* data = this->memoryMap + (offset - this->mappedRange.getOffset());
        this->fill(offset, size, data);

        if (!this->ownsMemoryMap)
        {
            auto* gfx = Module::getInstance<Graphics>(Module::M_GRAPHICS);
            gfx->releaseBufferMapMemory(this->memoryMap);
            this->memoryMap = nullptr;
        }
    }

    bool Buffer::fill(size_t offset, size_t size, const void* data)
    {
        if (size == 0 || this->isImmutable() || this->dataUsage == BUFFERDATAUSAGE_READBACK)
            return false;

        size_t bufferSize = this->getSize();

        if (!Range(0, bufferSize).contains(Range(offset, size)))
            return false;

        if (this->supportsOrphan() && size == bufferSize)
        {
            if (this->usage & BUFFERUSAGEFLAG_VERTEX)
                c3d.bufferDataSubOrphan(&this->buffer, (uint8_t*)data, this->arrayStride, size);
            else
                std::memcpy(this->bytes, data, size);
        }
        else
        {
            if (this->usage & BUFFERUSAGEFLAG_VERTEX)
                c3d.bufferDataSubOrphan(&this->buffer, (uint8_t*)data + offset, this->arrayStride, size);
            else
                std::memcpy(this->bytes + offset, data, size);
        }

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

        const char* src = (const char*)this->bytes + sourceOffset;
        destination->fill(destOffset, size, src);
    }
} // namespace love
