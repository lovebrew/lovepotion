#pragma once

#include "common/Object.hpp"

#include "modules/graphics/vertex.hpp"

namespace love
{
    class StreamBufferBase : public Object, public Resource
    {
      public:
        struct MapInfo
        {
            uint8_t* data = nullptr;
            size_t size   = 0;

            MapInfo()
            {}

            MapInfo(uint8_t* data, size_t size) : data(data), size(size)
            {}
        };

        StreamBufferBase(BufferUsage usage, size_t size) :
            usage(usage),
            data(nullptr),
            bufferSize(size),
            frameGPUReadOffset(0)
        {}

        virtual ~StreamBufferBase()
        {}

        MapInfo map()
        {
            return MapInfo(this->data, this->bufferSize - this->frameGPUReadOffset);
        }

        size_t unmap(size_t)
        {
            return (size_t)this->data;
        }

        size_t getSize() const
        {
            return this->bufferSize;
        }

        size_t getUsableSize() const
        {
            return this->bufferSize - this->frameGPUReadOffset;
        }

        BufferUsage getMode() const
        {
            return this->usage;
        }

        void markUsed(size_t size)
        {
            this->frameGPUReadOffset += size;
        }

        void nextFrame()
        {
            this->frameGPUReadOffset = 0;
        }

      protected:
        BufferUsage usage;

        uint8_t* data;
        size_t bufferSize;

        size_t frameGPUReadOffset;
    };
} // namespace love
