#include <utilities/driver/renderer/drawcommand.hpp>

#include <utilities/driver/renderer_ext.hpp>

namespace love
{
    std::span<vertex::Vertex> DrawCommand::AllocateVertices(size_t count)
    {
        return Renderer<Console::Which>::AllocateVertices(count);
    }
} // namespace love
