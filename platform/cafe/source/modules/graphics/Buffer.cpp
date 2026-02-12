#include "modules/graphics/Buffer.hpp"

#include "common/Exception.hpp"
#include "driver/display/UniqueBuffer.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/vertex.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <limits>

#include "common/debug.hpp"

namespace love
{
    static inline GX2RResourceFlags getBufferUsage(BufferUsage usage)
    {
        if (usage == BUFFERUSAGE_VERTEX)
            return GX2R_RESOURCE_BIND_VERTEX_BUFFER;

        return GX2R_RESOURCE_BIND_INDEX_BUFFER;
    }

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
    }

    bool Buffer::loadVolatile()
    {
        if (GX2RBufferExists(&this->buffer))
            return true;

        return this->load(nullptr);
    }

    void Buffer::unloadVolatile()
    {
        this->mapped = false;
        // GX2RDestroyBufferEx(&this->buffer, GX2R_RESOURCE_BIND_NONE);
        std::free(this->staging);
        this->staging = nullptr;
    }

    bool Buffer::load(const void* data)
    {

        this->bytes = (uint8_t*)std::malloc(this->getSize());
        if (this->bytes == nullptr)
            return false;

        GX2RResourceFlags flags;
        UniqueBuffer::getConstant(this->mapUsage, flags);

        this->buffer.elemCount = this->arrayLength;
        this->buffer.elemSize  = this->arrayStride;
        this->buffer.flags     = flags;

        if (!GX2RCreateBuffer(&this->buffer))
            return false;

        if (data != nullptr)
            std::memcpy(this->bytes, data, this->getSize());
        else
            std::memset(this->bytes, 0, this->getSize());

        UniqueBuffer buffer(&this->buffer);
        buffer.fill(this->getSize(), this->bytes);

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
            return (void*)((uint8_t*)this->bytes + offset);

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
            std::memcpy(this->bytes, data, bufferSize);
        else
            std::memcpy((uint8_t*)this->bytes + offset, data, size);

        UniqueBuffer buffer(&this->buffer);
        buffer.fill(size, this->bytes);

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
