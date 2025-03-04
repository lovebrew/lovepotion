#pragma once

#include "common/Object.hpp"

#include "modules/graphics/Resource.hpp"
#include "modules/graphics/vertex.hpp"

#include <type_traits>

namespace love
{
    template<typename T>
    struct MapInfo
    {
        T* data     = nullptr;
        size_t size = 0;

        MapInfo()
        {}

        MapInfo(T* data, size_t size) : data(data), size(size)
        {}
    };

    static constexpr int BUFFER_FRAMES = 2;

    template<typename T>
    class StreamBufferBase : public Object, public Resource
    {
      public:
        virtual ~StreamBufferBase()
        {}

        size_t getSize() const
        {
            return this->bufferSize;
        }

        BufferUsage getMode() const
        {
            return this->mode;
        }

        size_t getUsableSize() const
        {
            return this->bufferSize - this->frameGPUReadOffset;
        }

        // MapInfo<T> map(size_t) = 0;

        virtual size_t unmap(size_t)
        {
            return this->index;
        }

        void markUsed(int count)
        {
            this->index += count;
            this->frameGPUReadOffset += (count * sizeof(T));
        }

        void nextFrame()
        {
            this->index              = 0;
            this->frameGPUReadOffset = 0;
        }

      protected:
        StreamBufferBase(BufferUsage mode, size_t size) :
            bufferSize(size),
            frameGPUReadOffset(0),
            index(0),
            mode(mode)
        {}

        size_t bufferSize;
        size_t frameGPUReadOffset;
        int index;

        BufferUsage mode;
    };
} // namespace love
