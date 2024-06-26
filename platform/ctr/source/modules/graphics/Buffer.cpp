#include "modules/graphics/Buffer.hpp"

#include <3ds.h>

namespace love
{
    Buffer::Buffer(GraphicsBase* graphics, const Settings& settings,
                   const std::vector<DataDeclaration>& format, const void* data, size_t size,
                   size_t arraylength) :
        BufferBase(graphics, settings, format, size, arraylength),
    {
        this->size  = this->getSize();
        arraylength = this->getArrayLength();

        if (usageFlags & BUFFERUSAGEFLAG_VERTEX)
        {
            mapUsage     = BUFFERUSAGE_VERTEX;
            this->buffer = new C3D_BufInfo();
        }
        else if (usageFlags & BUFFERUSAGEFLAG_INDEX)
            mapUsage = BUFFERUSAGE_INDEX;

        if (dataUsage == BUFFERDATAUSAGE_STREAM)
            this->ownsMemoryMap = true;

        std::vector<uint8_t> empty {};

        try
        {
            empty.resize(this->getSize());
            data = empty.data();
        }
        catch (std::exception&)
        {
            data = nullptr;
        }

        if (!this->load(data))
        {
            this->unloadVolatile();
            throw love::Exception("Could not create buffer with %d bytes (out of VRAM?)", size);
        }
    }

    Buffer::~Buffer()
    {
        this->unloadVolatile();
        if (this->memoryMap != nullptr && this->ownsMemoryMap)
            linearFree(this->memoryMap);
    }

    bool Buffer::loadVolatile()
    {
        if (this->buffer != nullptr)
            return true;

        return this->load(nullptr);
    }

    void Buffer::unloadVolatile()
    {
        this->mapped = false;

        if (this->buffer != nullptr)
            delete this->buffer;

        this->buffer = nullptr;

        if (this->texture != nullptr)
            delete this->texture;

        this->texture = nullptr;
    }

    bool Buffer::load(const void* data)
    {
        if (this->mapUsage != BUFFERUSAGE_VERTEX)
            return true;

        BufInfo_Init(this->buffer);

        if (data != nullptr)
            BufInfo_Add(this->buffer, data, this->getArrayStride(), 3, 0x210);
    }

    void* Buffer::map(MapType map, size_t offset, size_t size)
    {
        if (size == 0)
            return nullptr;

        const bool isReadback = this->dataUsage == BUFFERDATAUSAGE_READBACK;
        if (map == MAP_WRITE_INVALIDATE && (this->isImmutable() || isReadback))
            return nullptr;

        if (map == MAP_READ_ONLY && this->dataUsage != BUFFERDATAUSAGE_READBACK)
            return nullptr;

        Range range(offset, size);

        if (!Range(0, this->getSize()).contains(range))
            return nullptr;

        char* data = nullptr;

        if (map == MAP_READ_ONLY)
        {
        }
        else if (this->ownsMemoryMap)
        {
            if (this->memoryMap == nullptr)
                this->memoryMap = (char*)linearAlloc(this->getSize());

            data = this->memoryMap;
        }
        else
        {
        }

        if (data != nullptr)
        {
            this->mapped      = true;
            this->mappedType  = map;
            this->mappedRange = range;

            if (!this->ownsMemoryMap)
                this->memoryMap = data;
        }

        return data;
    }
} // namespace love
