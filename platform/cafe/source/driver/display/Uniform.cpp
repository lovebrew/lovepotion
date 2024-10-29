#include "driver/display/Uniform.hpp"

namespace love
{
    glm::mat4 createTransposedSwappedMatrix(const Matrix4& matrix)
    {
        const uint32_t count = sizeof(glm::mat4) / sizeof(uint32_t);

        // Transpose the input matrix to convert from row-major to column-major
        glm::mat4 sourceMatrix = glm::transpose(glm::make_mat4(matrix.getElements()));

        // Create a new matrix to hold the endian-swapped version
        glm::mat4 resultMatrix;
        uint32_t* destination  = (uint32_t*)glm::value_ptr(resultMatrix);
        const uint32_t* source = (const uint32_t*)glm::value_ptr(sourceMatrix);

        for (int i = 0; i < count; ++i)
            destination[i] = __builtin_bswap32(source[i]);

        return resultMatrix;
    }
} // namespace love
