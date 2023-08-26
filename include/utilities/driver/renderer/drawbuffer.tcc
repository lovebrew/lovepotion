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
#if defined(__3DS__)
        DrawBuffer() : valid(false)
        {}

        DrawBuffer(size_t count) : size(count * VERTEX_SIZE), valid(true)
        {}
#else
        DrawBuffer() : vertices(nullptr), valid(false)
        {}

        DrawBuffer(size_t count) : size(count * VERTEX_SIZE), vertices(nullptr), valid(true)
        {}
#endif
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

#if defined(__3DS__)
        static inline Vertex* vertices = nullptr;
#else
        Vertex* vertices;
#endif

        bool valid;
        static inline bool initialized = false;
    };
} // namespace love