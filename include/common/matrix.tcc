#pragma once

#include "console.hpp"
#include "math.hpp"

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Matrix4
    {
      public:
        Matrix4();

        Matrix4(const float (&elements)[16], bool columnMajor)
        {
            static_assert(sizeof(elements) == sizeof(float[16]));
            std::memcpy(this->elements, elements, sizeof(elements));
        }

      private:
        float elements[16];
    };
} // namespace love
