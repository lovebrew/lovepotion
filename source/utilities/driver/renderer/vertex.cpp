#include <utilities/driver/renderer/vertex.hpp>

using namespace love::vertex;

template<typename T>
static void fillIndicesT(TriangleIndexMode mode, T start, T count, T* indices)
{
    switch (mode)
    {
        case TRIANGLEINDEX_NONE:
            break;
        case TRIANGLEINDEX_STRIP:
        {
            int i = 0;
            for (T index = 0; index < count - 2; index++)
            {
                indices[i++] = start + index;
                indices[i++] = start + index + 1 + (index & 1);
                indices[i++] = start + index + 2 - (index & 1);
            }
            break;
        }
        case TRIANGLEINDEX_FAN:
        {
            int i = 0;
            for (T index = 2; index < count; index++)
            {
                indices[i++] = start;
                indices[i++] = start + index + 1;
                indices[i++] = start + index;
            }
            break;
        }
        case TRIANGLEINDEX_QUADS:
        {
            // 0---3
            // | \ |
            // 1---2

            int total = count / 4;
            for (int index = 0; index < total; index++)
            {
                const int i = index * 6;
                T vertex    = T(start + index * 4);

                indices[i + 0] = vertex + 0;
                indices[i + 1] = vertex + 1;
                indices[i + 2] = vertex + 2;

                indices[i + 3] = vertex + 2;
                indices[i + 4] = vertex + 3;
                indices[i + 5] = vertex + 0;
            }
            break;
        }
    }
}

// clang-format off
// Order here relies on order of DataFormat enum.
static const DataFormatInfo dataFormatInfo[]
{
    // baseType, isMatrix, components, rows, columns, componentSize, size
    { DATA_BASETYPE_FLOAT, false, 1, 0, 0, 4, 4  }, // DATAFORMAT_FLOAT
    { DATA_BASETYPE_FLOAT, false, 2, 0, 0, 4, 8  }, // DATAFORMAT_FLOAT_VEC2
    { DATA_BASETYPE_FLOAT, false, 3, 0, 0, 4, 12 }, // DATAFORMAT_FLOAT_VEC3
    { DATA_BASETYPE_FLOAT, false, 4, 0, 0, 4, 16 }, // DATAFORMAT_FLOAT_VEC4

    { DATA_BASETYPE_FLOAT, true, 0, 2, 2, 4, 16 }, // DATAFORMAT_FLOAT_MAT2X2
    { DATA_BASETYPE_FLOAT, true, 0, 2, 3, 4, 24 }, // DATAFORMAT_FLOAT_MAT2X3
    { DATA_BASETYPE_FLOAT, true, 0, 2, 4, 4, 32 }, // DATAFORMAT_FLOAT_MAT2X4

    { DATA_BASETYPE_FLOAT, true, 0, 3, 2, 4, 24 }, // DATAFORMAT_FLOAT_MAT3X2
    { DATA_BASETYPE_FLOAT, true, 0, 3, 3, 4, 36 }, // DATAFORMAT_FLOAT_MAT3X3
    { DATA_BASETYPE_FLOAT, true, 0, 3, 4, 4, 48 }, // DATAFORMAT_FLOAT_MAT3X4

    { DATA_BASETYPE_FLOAT, true, 0, 4, 2, 4, 32 }, // DATAFORMAT_FLOAT_MAT4X2
    { DATA_BASETYPE_FLOAT, true, 0, 4, 3, 4, 48 }, // DATAFORMAT_FLOAT_MAT4X3
    { DATA_BASETYPE_FLOAT, true, 0, 4, 4, 4, 64 }, // DATAFORMAT_FLOAT_MAT4X4

    { DATA_BASETYPE_INT, false, 1, 0, 0, 4, 4  }, // DATAFORMAT_INT32
    { DATA_BASETYPE_INT, false, 2, 0, 0, 4, 8  }, // DATAFORMAT_INT32_VEC2
    { DATA_BASETYPE_INT, false, 3, 0, 0, 4, 12 }, // DATAFORMAT_INT32_VEC3
    { DATA_BASETYPE_INT, false, 4, 0, 0, 4, 16 }, // DATAFORMAT_INT32_VEC4

    { DATA_BASETYPE_UINT, false, 1, 0, 0, 4, 4  }, // DATAFORMAT_UINT32
    { DATA_BASETYPE_UINT, false, 2, 0, 0, 4, 8  }, // DATAFORMAT_UINT32_VEC2
    { DATA_BASETYPE_UINT, false, 3, 0, 0, 4, 12 }, // DATAFORMAT_UINT32_VEC3
    { DATA_BASETYPE_UINT, false, 4, 0, 0, 4, 16 }, // DATAFORMAT_UINT32_VEC4

    { DATA_BASETYPE_SNORM, false, 4, 0, 0, 1, 4 }, // DATAFORMAT_SNORM8_VEC4
    { DATA_BASETYPE_UNORM, false, 4, 0, 0, 1, 4 }, // DATAFORMAT_UNORM8_VEC4
    { DATA_BASETYPE_INT,   false, 4, 0, 0, 1, 4 }, // DATAFORMAT_INT8_VEC4
    { DATA_BASETYPE_UINT,  false, 4, 0, 0, 1, 4 }, // DATAFORMAT_UINT8_VEC4

    { DATA_BASETYPE_SNORM, false, 2, 0, 0, 2, 4 }, // DATAFORMAT_SNORM16_VEC2
    { DATA_BASETYPE_SNORM, false, 4, 0, 0, 2, 8 }, // DATAFORMAT_SNORM16_VEC4

    { DATA_BASETYPE_UNORM, false, 2, 0, 0, 2, 4 }, // DATAFORMAT_UNORM16_VEC2
    { DATA_BASETYPE_UNORM, false, 4, 0, 0, 2, 8 }, // DATAFORMAT_UNORM16_VEC4

    { DATA_BASETYPE_INT, false, 2, 0, 0, 2, 4 }, // DATAFORMAT_INT16_VEC2
    { DATA_BASETYPE_INT, false, 4, 0, 0, 2, 8 }, // DATAFORMAT_INT16_VEC4

    { DATA_BASETYPE_UINT, false, 1, 0, 0, 2, 2 }, // DATAFORMAT_UINT16
    { DATA_BASETYPE_UINT, false, 2, 0, 0, 2, 4 }, // DATAFORMAT_UINT16_VEC2
    { DATA_BASETYPE_UINT, false, 4, 0, 0, 2, 8 }, // DATAFORMAT_UINT16_VEC4

    { DATA_BASETYPE_BOOL, false, 1, 0, 0, 4, 4  }, // DATAFORMAT_BOOL
    { DATA_BASETYPE_BOOL, false, 2, 0, 0, 4, 8  }, // DATAFORMAT_BOOL_VEC2
    { DATA_BASETYPE_BOOL, false, 3, 0, 0, 4, 12 }, // DATAFORMAT_BOOL_VEC3
    { DATA_BASETYPE_BOOL, false, 4, 0, 0, 4, 16 }, // DATAFORMAT_BOOL_VEC4
};
// clang-format on
static_assert((sizeof(dataFormatInfo) / sizeof(DataFormatInfo)) == DATAFORMAT_MAX_ENUM,
              "dataFormatInfo array size must match number of DataFormat enum values.");

const DataFormatInfo& GetDataFormatInfo(DataFormat format)
{
    return dataFormatInfo[format];
}

void love::vertex::FillIndices(TriangleIndexMode mode, uint16_t start, uint16_t count,
                               uint16_t* indices)
{
    fillIndicesT(mode, start, count, indices);
}

void love::vertex::FillIndices(TriangleIndexMode mode, uint32_t start, uint32_t count,
                               uint32_t* indices)
{
    fillIndicesT(mode, start, count, indices);
}
