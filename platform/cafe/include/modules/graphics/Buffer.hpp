#pragma once

#include "common/Range.hpp"

#include "modules/graphics/Buffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <gx2r/buffer.h>

namespace love
{
    class Buffer final : public BufferBase, public Volatile
    {
      public:
        Buffer(GraphicsBase* graphics, const Settings& settings, const BufferFormat& format, const void* data,
               size_t size, size_t length);

        virtual ~Buffer();

        bool loadVolatile() override;

        void unloadVolatile() override;

        void* map(MapType map, size_t offset, size_t size) override;

        void unmap(size_t usedoffset, size_t usedsize) override;

        bool fill(size_t offset, size_t size, const void* data) override;

        bool supportsOrphan() const;

        void copyTo(BufferBase* dest, size_t sourceoffset, size_t destoffset, size_t size) override;

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)std::addressof(this->buffer);
        };

        ptrdiff_t getTexelBufferHandle() const override
        {
            return 0;
        }

        BufferUsage getMapUsage() const
        {
            return this->mapUsage;
        }

      private:
        bool load(const void* data);

        void clearInternal(size_t offset, size_t size) override;

        GX2RBuffer buffer;

        BufferUsage mapUsage = BUFFERUSAGE_VERTEX;
        uint8_t* staging     = nullptr;

        bool mapped        = false;
        bool ownsMemoryMap = false;

        MapType mappedType;
        Range mappedRange;
    };
} // namespace love
