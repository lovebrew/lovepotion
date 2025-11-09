#include "common/Exception.hpp"
#include "common/Matrix.hpp"

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Mesh.hpp"

#include <algorithm>
#include <limits>

namespace love
{
    // clang-format off
    static_assert(offsetof(Vertex, x) == sizeof(float) * 0, "Incorrect position offset in Vertex struct");
    static_assert(offsetof(Vertex, s) == sizeof(float) * 2, "Incorrect texture coordinate offset in Vertex struct");
    static_assert(offsetof(Vertex, color) == sizeof(float) * 4, "Incorrect color offset in Vertex struct");
    // clang-format on

    std::vector<BufferBase::DataDeclaration> Mesh::getDefaultVertexFormat()
    {
        return BufferBase::getCommonFormatDeclaration(CommonFormat::XYf_STf_RGBAf);
    }

    Type Mesh::type = Type("Mesh", &Drawable::type);

    Mesh::Mesh(GraphicsBase* graphics, const BufferBase::BufferFormat& vertexFormat, const void* data,
               size_t dataSize, PrimitiveType mode, BufferDataUsage usage) :
        drawMode(mode)
    {
        try
        {
            this->vertexData = new uint8_t[dataSize];
        }
        catch (std::exception&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        std::memcpy(this->vertexData, data, dataSize);

        BufferBase::Settings settings(BUFFERUSAGE_VERTEX, usage);
    }
} // namespace love
