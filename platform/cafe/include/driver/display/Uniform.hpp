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

        void debug()
        {
            const auto count = sizeof(glm::mat4) / sizeof(uint32_t);

            uint32_t* model = (uint32_t*)glm::value_ptr(this->modelView);
            for (size_t index = 0; index < count; index++)
                std::printf("modelView[%zu] = %u\n", index, model[index]);

            uint32_t* projection = (uint32_t*)glm::value_ptr(this->projection);
            for (size_t index = 0; index < count; index++)
                std::printf("projection[%zu] = %u\n", index, projection[index]);
        }
    };

} // namespace love
