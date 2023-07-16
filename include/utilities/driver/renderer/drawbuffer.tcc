#pragma once

#include <common/console.hpp>

#include <utilities/driver/renderer/vertex.hpp>

namespace love
{
    using namespace vertex;

    template<Console::Platform T = Console::ALL>
    struct DrawBuffer
    {
      public:
        DrawBuffer() : vertices(nullptr), valid(true)
        {}

        DrawBuffer(size_t count) : size(count * VERTEX_SIZE)
        {}

        DrawBuffer(DrawBuffer&&) = delete;

        DrawBuffer(const DrawBuffer&) = delete;

        DrawBuffer& operator=(const DrawBuffer&) = delete;

        Vertex* GetVertices()
        {
            return this->vertices;
        }

        bool IsValid() const
        {
            return this->valid;
        }

      protected:
        size_t size;
        Vertex* vertices;

        bool valid;
    };
} // namespace love