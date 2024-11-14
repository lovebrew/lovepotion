#include "driver/display/Uniform.hpp"

namespace love
{
    void debugUniform(Uniform* uniform)
    {
        const auto count = sizeof(glm::mat4) / sizeof(uint32_t);

        uint32_t* model = (uint32_t*)glm::value_ptr(uniform->modelView);
        for (size_t index = 0; index < count; index++)
            std::printf("modelView[%zu] = %u\n", index, model[index]);

        uint32_t* projection = (uint32_t*)glm::value_ptr(uniform->projection);
        for (size_t index = 0; index < count; index++)
            std::printf("projection[%zu] = %u\n", index, projection[index]);
    }
} // namespace love
