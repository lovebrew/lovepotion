#if defined(__SWITCH__)
    #include "driver/display/deko.hpp"
#endif

#include "driver/graphics/DrawCommand.hpp"

namespace love
{
    StreamBuffer<Vertex>* newVertexBuffer(size_t size)
    {
        auto* buffer = new StreamBuffer<Vertex>(BUFFERUSAGE_VERTEX, size);
#if defined(__SWITCH__)
        buffer->allocate(d3d.getMemoryPool(deko3d::MEMORYPOOL_DATA));
#endif
        return buffer;
    }

    StreamBuffer<uint16_t>* newIndexBuffer(size_t size)
    {
        auto* buffer = new StreamBuffer<uint16_t>(BUFFERUSAGE_INDEX, size);
#if defined(__SWITCH__)
        buffer->allocate(d3d.getMemoryPool(deko3d::MEMORYPOOL_DATA));
#endif
        return buffer;
    }
} // namespace love
