#include "driver/display/Uniform.hpp"

#include <bit>

namespace love
{
    glm::mat4 updateMatrix(const glm::mat4& matrix)
    {
        glm::mat4 out(1.0f);

        uint32_t* destination  = (uint32_t*)glm::value_ptr(out);
        const uint32_t* source = (const uint32_t*)glm::value_ptr(matrix);

        const size_t count = sizeof(glm::mat4) / sizeof(uint32_t);

        for (size_t i = 0; i < count; i++)
            destination[i] = __builtin_bswap32(source[i]);

        return out;
    }
} // namespace love
