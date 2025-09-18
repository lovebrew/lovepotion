#pragma once

#include "common/Object.hpp"

#include "modules/graphics/Resource.hpp"
#include "modules/graphics/vertex.hpp"

#include <type_traits>

#define MAX_FRAMES_IN_FLIGHT 2U

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

        virtual ~StreamBufferBase()
        {}

        virtual size_t getSize() const = 0;

        BufferUsage getMode() const
        {
            return this->mode;
        }

        size_t getUsableSize() const
        {
            return this->bufferSize - this->frameGPUReadOffset;
        }

        virtual size_t getGPUReadOffset() const = 0;

        virtual MapInfo map(size_t minSize) = 0;

        virtual size_t unmap(size_t usedSize) = 0;

        void markUsed(size_t usedSize)
        {
            this->frameGPUReadOffset += usedSize;
        }

        virtual void nextFrame() = 0;

      protected:
        StreamBufferBase(BufferUsage mode, size_t size) :
            bufferSize(size),
            frameGPUReadOffset(0),
            frameIndex(0),
            mode(mode)
        {}

        size_t bufferSize;
        size_t frameGPUReadOffset;
        int frameIndex;

        BufferUsage mode;
    };
} // namespace love
