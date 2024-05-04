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
            frameIndex(0),
            frameGPUReadOffset(0)
        {}

        virtual ~StreamBufferBase()
        {}

        MapInfo map()
        {
            MapInfo info {};

            info.size = this->bufferSize - this->frameGPUReadOffset;
            info.data = this->data + (this->frameIndex * this->bufferSize) + this->frameGPUReadOffset;

            return info;
        }

        virtual size_t unmap(size_t) = 0;

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
            this->frameIndex         = (this->frameIndex + 1) % BUFFER_FRAMES;
            this->frameGPUReadOffset = 0;
        }

      protected:
        static constexpr int BUFFER_FRAMES = 2;

        BufferUsage usage;

        uint8_t* data;
        size_t bufferSize;

        size_t frameIndex;
        size_t frameGPUReadOffset;
    };
} // namespace love
