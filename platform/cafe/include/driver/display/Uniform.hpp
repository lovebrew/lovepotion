#pragma once

#include "common/Matrix.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utility/logfile.hpp"

namespace love
{
    static glm::mat4 updateMatrix(const glm::mat4& matrix)
    {
        glm::mat4 out(1.0f);

        unsigned int* destination  = (unsigned int*)glm::value_ptr(out);
        const unsigned int* source = (const unsigned int*)glm::value_ptr(matrix);

        const size_t count = sizeof(glm::mat4) / sizeof(unsigned int);

        for (size_t i = 0; i < count; i++)
            destination[i] = __builtin_bswap32(source[i]);

        return glm::make_mat4(destination);
    }

    struct Uniform
    {
        glm::mat4 modelView;
        glm::mat4 projection;

      public:
        Uniform()
        {
            this->modelView = updateMatrix(glm::mat4(1.0f));
        }
    };
} // namespace love
