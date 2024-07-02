#pragma once

#include "common/Matrix.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace love
{
    glm::mat4 updateMatrix(const glm::mat4& matrix);

    struct Uniform
    {
        glm::mat4 modelView;
        glm::mat4 projection;
    };
} // namespace love
