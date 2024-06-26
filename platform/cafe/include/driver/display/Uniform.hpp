#pragma once

#include "common/Matrix.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace love
{
    static void updateGlmMatrix(glm::mat4& out, const glm::mat4& matrix)
    {
        unsigned int* destination = (unsigned int*)glm::value_ptr(out);
        unsigned int* source      = (unsigned int*)glm::value_ptr(matrix);

        const size_t count = sizeof(glm::mat4) / sizeof(unsigned int);

        for (size_t i = 0; i < count; i++)
            destination[i] = __builtin_bswap32(source[i]);
    }

    struct Uniform
    {
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 modelView  = glm::mat4(1.0f);
    };
} // namespace love
