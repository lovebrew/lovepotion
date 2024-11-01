#pragma once

#include "common/Exception.hpp"
#include "common/Matrix.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace love
{
    struct Uniform
    {
        glm::mat4 modelView;
        glm::mat4 projection;
    };

    void debugUniform(Uniform* uniform);

    static constexpr auto UNIFORM_SIZE = sizeof(Uniform);
} // namespace love
