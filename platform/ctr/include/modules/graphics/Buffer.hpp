#pragma once

#include "common/Range.hpp"

#include "modules/graphics/Buffer.tcc"
#include "modules/graphics/Volatile.hpp"

#include <citro3d.h>

namespace love
{
    class Buffer final : public BufferBase, public Volatile
    {
      public:
        Buffer(GraphicsBase* graphics, const Settings& settings, const std::vector<DataDeclaration>& format,
               const void* data, size_t size, size_t length);

        virtual ~Buffer();

        bool loadVolatile() override;

        void unloadVolatile() override;

        void* map(MapType map, size_t offset, size_t size) override;

        void unmap(size_t usedoffset, size_t usedsize) override;

        bool fill(size_t offset, size_t size, const void* data) override;

        void copyTo(BufferBase* dest, size_t sourceoffset, size_t destoffset, size_t size) override;

        ptrdiff_t getHandle() const override
        {
            return (ptrdiff_t)this->bytes.data();
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

        BufferUsage mapUsage = BUFFERUSAGE_VERTEX;
        std::vector<uint8_t> bytes;
        std::vector<uint8_t> staging;

        bool mapped        = false;
        bool ownsMemoryMap = false;

        MapType mappedType;
        Range mappedRange;
    };
} // namespace love
