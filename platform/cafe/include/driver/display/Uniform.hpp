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

        void update(const Matrix4& matrix)
        {
            uint32_t* dstModel = (uint32_t*)glm::value_ptr(this->modelView);
            const size_t count = sizeof(glm::mat4) / sizeof(uint32_t);

            const auto* model = (uint32_t*)matrix.getElements();
            for (size_t index = 0; index < count; index++)
                dstModel[index] = __builtin_bswap32(model[index]);
        }
    };

    void debugUniform(Uniform* uniform);

    static constexpr auto UNIFORM_SIZE = sizeof(Uniform);
} // namespace love
