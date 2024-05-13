#pragma once

#include "common/Object.hpp"

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
            return this->usage;
        }

        size_t getUsableSize() const
        {
            return this->bufferSize - this->frameGPUReadOffset;
        }

        /*
        ** Get the offset of the buffer that the GPU can read from.
        ** This is the offset of the current frame.
        ** @return The offset in bytes.
        */
        size_t getGPUReadOffset() const
        {
            return (this->frameIndex * this->bufferSize) + this->frameGPUReadOffset;
        }

        int getBufferIndex() const
        {
            return (this->frameIndex * this->bufferSize) + this->index;
        }

        /*
        ** Map the buffer for writing.
        ** @return A pointer to the buffer.
        */
        virtual MapInfo<T> map(size_t) = 0;

        /*
        ** Unmap the buffer.
        ** @return Offset of the buffer that was written to.
        */
        virtual size_t unmap(size_t) = 0;

        /*
        ** Mark the buffer as used.
        ** @param count The count of T of the buffer that was used.
        */
        virtual void markUsed(int count) = 0;

        void clear()
        {}

        /*
        ** Advance to the next frame.
        */
        virtual void nextFrame() = 0;

      protected:
        StreamBufferBase(BufferUsage usage, size_t size) :
            bufferSize(size),
            frameGPUReadOffset(0),
            frameIndex(0),
            index(0),
            usage(usage)
        {}

        size_t bufferSize;
        size_t frameGPUReadOffset;
        size_t frameIndex;

        int index;

        BufferUsage usage;
    };
} // namespace love
