#pragma once

#include "deko3d/mapper.h"
#include "deko3d/resource.h"
#include "deko3d/vertex.h"

namespace love
{
    class Buffer : public Resource
    {
      public:
        enum MapFlags
        {
            MAP_EXPLICIT_RANGE_MODIFY = (1 << 0),
            MAP_READ                  = (1 << 1)
        };

        Buffer(size_t size, const void* data, vertex::BufferType type, vertex::Usage usage,
               uint32_t mapFlags);

        virtual ~Buffer();

        size_t GetSize() const
        {
            return this->size;
        };

        vertex::BufferType GetType() const
        {
            return this->type;
        };

        vertex::Usage GetUsage() const
        {
            return this->usage;
        };

        bool IsMapped() const
        {
            return this->isMapped;
        };

        void* Map();

        void Unmap();

        void SetMappedRangeModified(size_t offset, size_t size);

        void Fill(size_t offset, size_t size, const void* data);

        void CopyTo(size_t offset, size_t size, Buffer* other, size_t otherOffset);

        uint32_t GetMapFlags() const
        {
            return this->mapFlags;
        };

        ptrdiff_t GetHandle() const override;

        bool LoadVolatile();

        void UnloadVolatile();

      private:
        bool Load(bool restore);

        bool Unload();

        void UnmapStatic(size_t offset, size_t size);

        void UnmapStream();

        size_t size;
        vertex::BufferType type;
        vertex::Usage usage;
        uint32_t mapFlags;
        bool isMapped;

        char* memoryMap;
        size_t modifiedStart;
        size_t modifiedEnd;

        dk::MemBlock memblock;
    };
} // namespace love
