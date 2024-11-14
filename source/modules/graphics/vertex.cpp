#include "modules/graphics/vertex.hpp"

#include <cstdio>

namespace love
{
    void debugVertices(Vertex* vertices, size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            const auto& v = vertices[i];

            std::printf("Vertex %zu:\n", i);
            std::printf("  Position: %.2f, %.2f\n", v.x, v.y);
            std::printf("  Texture Coordinates: %f, %f\n", v.s, v.t);
            std::printf("  Color: %.2f, %.2f, %.2f, %.2f\n", v.color.r, v.color.g, v.color.b, v.color.a);
        }
    }

    void debugIndices(uint16_t* indices, size_t count)
    {
        std::printf("Indices (%zu): ", count);
        for (size_t index = 0; index < count; index++)
            std::printf("%hu ", indices[index]);

        std::printf("\n");
    }

    int getIndexCount(TriangleIndexMode mode, int vertexCount)
    {
        switch (mode)
        {
            case TRIANGLEINDEX_NONE:
                return 0;
            case TRIANGLEINDEX_STRIP:
            case TRIANGLEINDEX_FAN:
                return 3 * (vertexCount - 2);
            case TRIANGLEINDEX_QUADS:
                return vertexCount * 6 / 4;
        }

        return 0;
    }

    size_t getFormatStride(CommonFormat format)
    {
        switch (format)
        {
            case CommonFormat::NONE:
                return 0;
            case CommonFormat::XYf:
                return sizeof(float) * 2;
            case CommonFormat::XYZf:
                return sizeof(float) * 3;
            case CommonFormat::RGBAf:
                return sizeof(float) * 4;
            case CommonFormat::STf_RGBAf:
                return sizeof(STf_RGBAf);
            case CommonFormat::STPf_RGBAf:
                return sizeof(STPf_RGBAf);
            case CommonFormat::XYf_STf:
                return sizeof(XYf_STf);
            case CommonFormat::XYf_STPf:
                return sizeof(XYf_STPf);
            case CommonFormat::XYf_STf_RGBAf:
                return sizeof(XYf_STf_RGBAf);
            case CommonFormat::XYf_STus_RGBAf:
                return sizeof(XYf_STus_RGBAf);
            case CommonFormat::XYf_STPf_RGBAf:
                return sizeof(XYf_STPf_RGBAf);
        }

        return 0;
    }

    template<typename T>
    static void fillIndicesT(TriangleIndexMode mode, T vertexStart, T vertexCount, T* indices)
    {
        switch (mode)
        {
            case TRIANGLEINDEX_NONE:
                break;
            case TRIANGLEINDEX_STRIP:
            {
                int i = 0;
                for (T index = 0; index < vertexCount - 2; index++)
                {
                    indices[i++] = vertexStart + index;
                    indices[i++] = vertexStart + index + 1 + (index & 1);
                    indices[i++] = vertexStart + index + 2 - (index & 1);
                }

                break;
            }
            case TRIANGLEINDEX_FAN:
            {
                int i = 0;
                for (T index = 2; index < vertexCount; index++)
                {
                    indices[i++] = vertexStart;
                    indices[i++] = vertexStart + index - 1;
                    indices[i++] = vertexStart + index;
                }

                break;
            }
            case TRIANGLEINDEX_QUADS:
            {
                // 0---2
                // | / |
                // 1---3
                int count = vertexCount / 4;
                for (int i = 0; i < count; i++)
                {
                    int ii = i * 6;
                    T vi   = T(vertexStart + i * 4);

                    indices[ii + 0] = vi + 0;
                    indices[ii + 1] = vi + 1;
                    indices[ii + 2] = vi + 2;

                    indices[ii + 3] = vi + 2;
                    indices[ii + 4] = vi + 1;
                    indices[ii + 5] = vi + 3;
                }

                break;
            }
        }
    }

    void fillIndices(TriangleIndexMode mode, uint16_t vertexStart, uint16_t vertexCount, uint16_t* indices)
    {
        fillIndicesT(mode, vertexStart, vertexCount, indices);
    }

    void fillIndices(TriangleIndexMode mode, uint32_t vertexStart, uint32_t vertexCount, uint32_t* indices)
    {
        fillIndicesT(mode, vertexStart, vertexCount, indices);
    }

    void VertexAttributes::setCommonFormat(CommonFormat format, uint8_t bufferindex)
    {
        setBufferLayout(bufferindex, (uint16_t)getFormatStride(format));

        switch (format)
        {
            case CommonFormat::NONE:
                break;
            case CommonFormat::XYf:
                set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC2, 0, bufferindex);
                break;
            case CommonFormat::XYZf:
                set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC3, 0, bufferindex);
                break;
            case CommonFormat::RGBAf:
                set(ATTRIB_COLOR, DATAFORMAT_UNORM8_VEC4, 0, bufferindex);
                break;
            case CommonFormat::STf_RGBAf:
                set(ATTRIB_TEXCOORD, DATAFORMAT_FLOAT_VEC2, 0, bufferindex);
                set(ATTRIB_COLOR, DATAFORMAT_FLOAT_VEC4, uint16_t(sizeof(float) * 2), bufferindex);
                break;
            case CommonFormat::STPf_RGBAf:
                set(ATTRIB_TEXCOORD, DATAFORMAT_FLOAT_VEC3, 0, bufferindex);
                set(ATTRIB_COLOR, DATAFORMAT_FLOAT_VEC4, uint16_t(sizeof(float) * 3), bufferindex);
                break;
            case CommonFormat::XYf_STf:
                set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC2, 0, bufferindex);
                set(ATTRIB_TEXCOORD, DATAFORMAT_FLOAT_VEC2, uint16_t(sizeof(float) * 2), bufferindex);
                break;
            case CommonFormat::XYf_STPf:
                set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC2, 0, bufferindex);
                set(ATTRIB_TEXCOORD, DATAFORMAT_FLOAT_VEC3, uint16_t(sizeof(float) * 2), bufferindex);
                break;
            case CommonFormat::XYf_STf_RGBAf:
                set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC2, 0, bufferindex);
                set(ATTRIB_TEXCOORD, DATAFORMAT_FLOAT_VEC2, uint16_t(sizeof(float) * 2), bufferindex);
                set(ATTRIB_COLOR, DATAFORMAT_FLOAT_VEC4, uint16_t(sizeof(float) * 4), bufferindex);
                break;
            case CommonFormat::XYf_STus_RGBAf:
                set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC2, 0, bufferindex);
                set(ATTRIB_TEXCOORD, DATAFORMAT_UNORM16_VEC2, uint16_t(sizeof(float) * 2), bufferindex);
                set(ATTRIB_COLOR, DATAFORMAT_FLOAT_VEC4, uint16_t(sizeof(float) * 2 + sizeof(uint16_t) * 2),
                    bufferindex);
                break;
            case CommonFormat::XYf_STPf_RGBAf:
                set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC2, 0, bufferindex);
                set(ATTRIB_TEXCOORD, DATAFORMAT_FLOAT_VEC3, uint16_t(sizeof(float) * 2), bufferindex);
                set(ATTRIB_COLOR, DATAFORMAT_FLOAT_VEC4, uint16_t(sizeof(float) * 5), bufferindex);
                break;
        }
    }

    bool VertexAttributes::operator==(const VertexAttributes& other) const
    {
        if (enableBits != other.enableBits || instanceBits != other.instanceBits)
            return false;

        uint32_t allbits = enableBits;
        uint32_t i       = 0;

        while (allbits)
        {
            if (isEnabled(i))
            {
                const auto& a = this->attributes[i];
                const auto& b = other.attributes[i];
                if (a.bufferIndex != b.bufferIndex || a.packedFormat != b.packedFormat ||
                    a.offsetFromVertex != b.offsetFromVertex)
                    return false;

                if (bufferLayouts[a.bufferIndex].stride != other.bufferLayouts[a.bufferIndex].stride)
                    return false;
            }

            i++;
            allbits >>= 1;
        }

        return true;
    }

    // Order here relies on order of DataFormat enum.
    static const DataFormatInfo dataFormatInfo[] {
        // baseType, isMatrix, components, rows, columns, componentSize, size
        { DATA_BASETYPE_FLOAT, false, 1, 0, 0, 4, 4 },  // DATAFORMAT_FLOAT
        { DATA_BASETYPE_FLOAT, false, 2, 0, 0, 4, 8 },  // DATAFORMAT_FLOAT_VEC2
        { DATA_BASETYPE_FLOAT, false, 3, 0, 0, 4, 12 }, // DATAFORMAT_FLOAT_VEC3
        { DATA_BASETYPE_FLOAT, false, 4, 0, 0, 4, 16 }, // DATAFORMAT_FLOAT_VEC4

        { DATA_BASETYPE_FLOAT, true, 4, 2, 2, 4, 16 }, // DATAFORMAT_FLOAT_MAT2X2
        { DATA_BASETYPE_FLOAT, true, 6, 2, 3, 4, 24 }, // DATAFORMAT_FLOAT_MAT2X3
        { DATA_BASETYPE_FLOAT, true, 8, 2, 4, 4, 32 }, // DATAFORMAT_FLOAT_MAT2X4

        { DATA_BASETYPE_FLOAT, true, 6, 3, 2, 4, 24 },  // DATAFORMAT_FLOAT_MAT3X2
        { DATA_BASETYPE_FLOAT, true, 9, 3, 3, 4, 36 },  // DATAFORMAT_FLOAT_MAT3X3
        { DATA_BASETYPE_FLOAT, true, 12, 3, 4, 4, 48 }, // DATAFORMAT_FLOAT_MAT3X4

        { DATA_BASETYPE_FLOAT, true, 8, 4, 2, 4, 32 },  // DATAFORMAT_FLOAT_MAT4X2
        { DATA_BASETYPE_FLOAT, true, 12, 4, 3, 4, 48 }, // DATAFORMAT_FLOAT_MAT4X3
        { DATA_BASETYPE_FLOAT, true, 16, 4, 4, 4, 64 }, // DATAFORMAT_FLOAT_MAT4X4

        { DATA_BASETYPE_INT, false, 1, 0, 0, 4, 4 },  // DATAFORMAT_INT32
        { DATA_BASETYPE_INT, false, 2, 0, 0, 4, 8 },  // DATAFORMAT_INT32_VEC2
        { DATA_BASETYPE_INT, false, 3, 0, 0, 4, 12 }, // DATAFORMAT_INT32_VEC3
        { DATA_BASETYPE_INT, false, 4, 0, 0, 4, 16 }, // DATAFORMAT_INT32_VEC4

        { DATA_BASETYPE_UINT, false, 1, 0, 0, 4, 4 },  // DATAFORMAT_UINT32
        { DATA_BASETYPE_UINT, false, 2, 0, 0, 4, 8 },  // DATAFORMAT_UINT32_VEC2
        { DATA_BASETYPE_UINT, false, 3, 0, 0, 4, 12 }, // DATAFORMAT_UINT32_VEC3
        { DATA_BASETYPE_UINT, false, 4, 0, 0, 4, 16 }, // DATAFORMAT_UINT32_VEC4

        { DATA_BASETYPE_SNORM, false, 4, 0, 0, 1, 4 }, // DATAFORMAT_SNORM8_VEC4
        { DATA_BASETYPE_UNORM, false, 4, 0, 0, 1, 4 }, // DATAFORMAT_UNORM8_VEC4
        { DATA_BASETYPE_INT, false, 4, 0, 0, 1, 4 },   // DATAFORMAT_INT8_VEC4
        { DATA_BASETYPE_UINT, false, 4, 0, 0, 1, 4 },  // DATAFORMAT_UINT8_VEC4

        { DATA_BASETYPE_SNORM, false, 2, 0, 0, 2, 4 }, // DATAFORMAT_SNORM16_VEC2
        { DATA_BASETYPE_SNORM, false, 4, 0, 0, 2, 8 }, // DATAFORMAT_SNORM16_VEC4

        { DATA_BASETYPE_UNORM, false, 2, 0, 0, 2, 4 }, // DATAFORMAT_UNORM16_VEC2
        { DATA_BASETYPE_UNORM, false, 4, 0, 0, 2, 8 }, // DATAFORMAT_UNORM16_VEC4

        { DATA_BASETYPE_INT, false, 2, 0, 0, 2, 4 }, // DATAFORMAT_INT16_VEC2
        { DATA_BASETYPE_INT, false, 4, 0, 0, 2, 8 }, // DATAFORMAT_INT16_VEC4

        { DATA_BASETYPE_UINT, false, 1, 0, 0, 2, 2 }, // DATAFORMAT_UINT16
        { DATA_BASETYPE_UINT, false, 2, 0, 0, 2, 4 }, // DATAFORMAT_UINT16_VEC2
        { DATA_BASETYPE_UINT, false, 4, 0, 0, 2, 8 }, // DATAFORMAT_UINT16_VEC4

        { DATA_BASETYPE_BOOL, false, 1, 0, 0, 4, 4 },  // DATAFORMAT_BOOL
        { DATA_BASETYPE_BOOL, false, 2, 0, 0, 4, 8 },  // DATAFORMAT_BOOL_VEC2
        { DATA_BASETYPE_BOOL, false, 3, 0, 0, 4, 12 }, // DATAFORMAT_BOOL_VEC3
        { DATA_BASETYPE_BOOL, false, 4, 0, 0, 4, 16 }, // DATAFORMAT_BOOL_VEC4
    };

    static_assert((sizeof(dataFormatInfo) / sizeof(DataFormatInfo)) == DATAFORMAT_MAX_ENUM,
                  "dataFormatInfo array size must match number of DataFormat enum values.");

    const DataFormatInfo& getDataFormatInfo(DataFormat format)
    {
        return dataFormatInfo[format];
    }

    IndexDataType getIndexDataTypeFromMax(size_t maxValue)
    {
        return maxValue > LOVE_UINT16_MAX ? INDEX_UINT32 : INDEX_UINT16;
    }

    DataFormat getIndexDataFormat(IndexDataType type)
    {
        return type == INDEX_UINT32 ? DATAFORMAT_UINT32 : DATAFORMAT_UINT16;
    }

    size_t getIndexDataSize(IndexDataType type)
    {
        switch (type)
        {
            case INDEX_UINT16:
                return sizeof(uint16_t);
            case INDEX_UINT32:
                return sizeof(uint32_t);
            default:
                return 0;
        }
    }
} // namespace love
