#include <cstddef>

#include <common/object.hpp>
#include <common/resource.hpp>

#include <utilities/driver/renderer/vertex.hpp>

namespace love
{
    class StreamBuffer : public Object, public Resource
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

        virtual ~StreamBuffer()
        {}

        size_t GetSize() const
        {
            return this->bufferSize;
        }

        vertex::BufferUsage GetMode() const
        {
            return this->mode;
        }

        size_t GetUsableSize() const
        {
            return this->bufferSize - this->frameGPUReadOffset;
        }

        virtual MapInfo Map(size_t minsize) = 0;

        virtual size_t Unmap(size_t usedsize) = 0;

        virtual void MarkUsed(size_t usedsize) = 0;

        virtual void NextFrame()
        {}

      protected:
        StreamBuffer(vertex::BufferUsage mode, size_t size) : bufferSize(size), frameGPUReadOffset(0), mode(mode)
        {}

        size_t bufferSize;
        size_t frameGPUReadOffset;
        vertex::BufferUsage mode;
    };
} // namespace love
