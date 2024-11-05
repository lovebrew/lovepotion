#pragma once

#include "common/Color.hpp"
#include "common/Map.hpp"
#include "common/int.hpp"

#include "common/Vector.hpp"

#include <cstring>

namespace love
{
    class Resource;

    enum BuiltinVertexAttribute
    {
        ATTRIB_POS = 0,
        ATTRIB_TEXCOORD,
        ATTRIB_COLOR,
        ATTRIB_MAX_ENUM
    };

    enum BuiltinVertexAttributeFlags
    {
        ATTRIBFLAG_POS      = 1 << ATTRIB_POS,
        ATTRIBFLAG_TEXCOORD = 1 << ATTRIB_TEXCOORD,
        ATTRIBFLAG_COLOR    = 1 << ATTRIB_COLOR
    };

    enum BufferUsage
    {
        BUFFERUSAGE_VERTEX = 0,
        BUFFERUSAGE_INDEX,
        BUFFERUSAGE_TEXEL,
        BUFFERUSAGE_UNIFORM,
        BUFFERUSAGE_SHADER_STORAGE,
        BUFFERUSAGE_INDIRECT_ARGUMENTS,
        BUFFERUSAGE_MAX_ENUM
    };

    enum BufferUsageFlags
    {
        BUFFERUSAGEFLAG_NONE               = 0,
        BUFFERUSAGEFLAG_VERTEX             = 1 << BUFFERUSAGE_VERTEX,
        BUFFERUSAGEFLAG_INDEX              = 1 << BUFFERUSAGE_INDEX,
        BUFFERUSAGEFLAG_TEXEL              = 1 << BUFFERUSAGE_TEXEL,
        BUFFERUSAGEFLAG_SHADER_STORAGE     = 1 << BUFFERUSAGE_SHADER_STORAGE,
        BUFFERUSAGEFLAG_INDIRECT_ARGUMENTS = 1 << BUFFERUSAGE_INDIRECT_ARGUMENTS,
    };

    enum IndexDataType
    {
        INDEX_UINT16,
        INDEX_UINT32,
        INDEX_MAX_ENUM
    };

    enum PrimitiveType
    {
        PRIMITIVE_TRIANGLES,
        PRIMITIVE_TRIANGLE_STRIP,
        PRIMITIVE_TRIANGLE_FAN,
        PRIMITIVE_POINTS,
        PRIMITIVE_MAX_ENUM
    };

    enum AttributeStep
    {
        STEP_PER_VERTEX,
        STEP_PER_INSTANCE,
        STEP_MAX_ENUM
    };

    enum CullMode
    {
        CULL_NONE,
        CULL_FRONT,
        CULL_BACK,
        CULL_MAX_ENUM
    };

    // The expected usage pattern of buffer data.
    enum BufferDataUsage
    {
        BUFFERDATAUSAGE_STREAM,
        BUFFERDATAUSAGE_DYNAMIC,
        BUFFERDATAUSAGE_STATIC,
        BUFFERDATAUSAGE_READBACK,
        BUFFERDATAUSAGE_MAX_ENUM
    };

    // Value types used when interfacing with the GPU (vertex and shader data).
    // The order of this enum affects the dataFormatInfo array.
    enum DataFormat
    {
        DATAFORMAT_FLOAT,
        DATAFORMAT_FLOAT_VEC2,
        DATAFORMAT_FLOAT_VEC3,
        DATAFORMAT_FLOAT_VEC4,

        DATAFORMAT_FLOAT_MAT2X2,
        DATAFORMAT_FLOAT_MAT2X3,
        DATAFORMAT_FLOAT_MAT2X4,

        DATAFORMAT_FLOAT_MAT3X2,
        DATAFORMAT_FLOAT_MAT3X3,
        DATAFORMAT_FLOAT_MAT3X4,

        DATAFORMAT_FLOAT_MAT4X2,
        DATAFORMAT_FLOAT_MAT4X3,
        DATAFORMAT_FLOAT_MAT4X4,

        DATAFORMAT_INT32,
        DATAFORMAT_INT32_VEC2,
        DATAFORMAT_INT32_VEC3,
        DATAFORMAT_INT32_VEC4,

        DATAFORMAT_UINT32,
        DATAFORMAT_UINT32_VEC2,
        DATAFORMAT_UINT32_VEC3,
        DATAFORMAT_UINT32_VEC4,

        DATAFORMAT_SNORM8_VEC4,
        DATAFORMAT_UNORM8_VEC4,
        DATAFORMAT_INT8_VEC4,
        DATAFORMAT_UINT8_VEC4,

        DATAFORMAT_SNORM16_VEC2,
        DATAFORMAT_SNORM16_VEC4,

        DATAFORMAT_UNORM16_VEC2,
        DATAFORMAT_UNORM16_VEC4,

        DATAFORMAT_INT16_VEC2,
        DATAFORMAT_INT16_VEC4,

        DATAFORMAT_UINT16,
        DATAFORMAT_UINT16_VEC2,
        DATAFORMAT_UINT16_VEC4,

        DATAFORMAT_BOOL,
        DATAFORMAT_BOOL_VEC2,
        DATAFORMAT_BOOL_VEC3,
        DATAFORMAT_BOOL_VEC4,

        DATAFORMAT_MAX_ENUM
    };

    enum DataBaseType
    {
        DATA_BASETYPE_FLOAT,
        DATA_BASETYPE_INT,
        DATA_BASETYPE_UINT,
        DATA_BASETYPE_SNORM,
        DATA_BASETYPE_UNORM,
        DATA_BASETYPE_BOOL,
        DATA_BASETYPE_MAX_ENUM
    };

    enum Winding
    {
        WINDING_CW,
        WINDING_CCW,
        WINDING_MAX_ENUM
    };

    enum TriangleIndexMode
    {
        TRIANGLEINDEX_NONE,
        TRIANGLEINDEX_STRIP,
        TRIANGLEINDEX_FAN,
        TRIANGLEINDEX_QUADS
    };

    enum class CommonFormat
    {
        NONE,           //< No format.
        XYf,            //< 2D position.
        XYZf,           //< 3D position.
        RGBAf,          //< 32-bit floating point RGBA color.
        STf_RGBAf,      //< 2D texture coordinates and 32-bit floating point RGBA color.
        STPf_RGBAf,     //< 3D texture coordinates and 32-bit floating point RGBA color.
        XYf_STf,        //< 2D position and 2D texture coordinates.
        XYf_STPf,       //< 2D position and 3D texture coordinates.
        XYf_STf_RGBAf,  //< 2D position, 2D texture coordinates and 32-bit floating point RGBA color.
        XYf_STus_RGBAf, //< 2D position, 2D unsigned short texture coordinates and 32-bit floating point RGBA
                        // color.
        XYf_RGBAf,
        XYf_STPf_RGBAf, //< 2D position, 3D texture coordinates and 32-bit floating point RGBA color.
    };

    struct STf_RGBAf
    {
        float s, t;
        Color color;
    };

    struct STPf_RGBAf
    {
        float s, t, p;
        Color color;
    };

    struct XYf_STf
    {
        float x, y;
        float s, t;
    };

    struct XYf_STPf
    {
        float x, y;
        float s, t, p;
    };

    struct XYf_STf_RGBAf
    {
        float x, y;
        float s, t;
        Color color;
    };

    struct XYf_STus_RGBAf
    {
        float x, y;
        uint16_t s, t;
        Color color;
    };

    struct XYf_STPf_RGBAf
    {
        float x, y;
        float s, t, p;
        Color color;
    };

    using Vertex = XYf_STf_RGBAf;

    static constexpr size_t VERTEX_SIZE = sizeof(Vertex);

    static constexpr size_t POSITION_OFFSET = offsetof(Vertex, x);
    static constexpr size_t TEXCOORD_OFFSET = offsetof(Vertex, s);
    static constexpr size_t COLOR_OFFSET    = offsetof(Vertex, color);

    inline void DEBUG_VERTEX(const Vertex& v)
    {
        std::printf("Position: %.2f, %.2f\n", v.x, v.y);
        std::printf("Texture Coordinates: %f, %f\n", v.s, v.t);
        std::printf("Color: %.2f, %.2f, %.2f, %.2f\n", v.color.r, v.color.g, v.color.b, v.color.a);
    }

    inline void DEBUG_VERTEX(const Vertex* v)
    {
        DEBUG_VERTEX(*v);
    }

    inline CommonFormat getSinglePositionFormat(bool is2D)
    {
        return is2D ? CommonFormat::XYf : CommonFormat::XYZf;
    }

    struct DataFormatInfo
    {
        DataBaseType baseType;
        bool isMatrix;
        int components;
        int matrixRows;
        int matrixColumns;
        size_t componentSize;
        size_t size;
    };

    const DataFormatInfo& getDataFormatInfo(DataFormat format);

    int getIndexCount(TriangleIndexMode mode, int vertexCount);

    size_t getFormatStride(CommonFormat format);

    void fillIndices(TriangleIndexMode mode, uint16_t vertexStart, uint16_t vertexCount, uint16_t* indices);

    void fillIndices(TriangleIndexMode mode, uint32_t vertexStart, uint32_t vertexCount, uint32_t* indices);

    struct BufferBindings
    {
        static const uint32_t MAX = 32;

        uint32_t useBits = 0;

        struct
        {
            Resource* buffer;
            size_t offset;
            int vertexCount;
        } info[MAX];

        void set(uint32_t index, Resource* r, size_t offset, int vertexCount)
        {
            useBits |= (1u << index);
            info[index] = { r, offset, vertexCount };
        }

        void disable(uint32_t index)
        {
            useBits &= (1u << index);
        }

        void clear()
        {
            useBits = 0;
        }
    };

    struct VertexAttributeInfo
    {
        uint16_t offsetFromVertex;
        uint8_t packedFormat;
        uint8_t bufferIndex;

        void setFormat(DataFormat format)
        {
            packedFormat = (uint8_t)format;
        }

        DataFormat getFormat() const
        {
            return (DataFormat)packedFormat;
        }
    };

    struct VertexBufferLayout
    {
        // Attribute step rate is stored outside this struct as a bitmask.
        uint16_t stride;
    };

    struct VertexAttributes
    {
        static constexpr uint32_t MAX = 32;

        uint32_t enableBits   = 0;
        uint32_t instanceBits = 0;

        VertexAttributeInfo attributes[MAX];
        VertexBufferLayout bufferLayouts[BufferBindings::MAX];

        VertexAttributes()
        {
            std::memset(this, 0, sizeof(VertexAttributes));
        }

        VertexAttributes(CommonFormat format, uint8_t bufferIndex)
        {
            std::memset(this, 0, sizeof(VertexAttributes));
            this->setCommonFormat(format, bufferIndex);
        }

        void set(uint32_t index, DataFormat format, uint16_t offsetFromVertex, uint8_t bufferIndex)
        {
            this->enableBits |= (1u << index);

            this->attributes[index].bufferIndex = bufferIndex;
            this->attributes[index].setFormat(format);
            this->attributes[index].offsetFromVertex = offsetFromVertex;
        }

        void setBufferLayout(uint32_t bufferIndex, uint16_t stride, AttributeStep step = STEP_PER_VERTEX)
        {
            uint32_t bufferBit = (1u << bufferIndex);

            if (step == STEP_PER_INSTANCE)
                this->instanceBits |= bufferBit;
            else
                this->instanceBits &= ~bufferBit;

            this->bufferLayouts[bufferIndex].stride = stride;
        }

        void disable(uint32_t index)
        {
            this->enableBits &= ~(1u << index);
        }

        void clear()
        {
            this->enableBits = 0;
        }

        bool isEnabled(uint32_t index) const
        {
            return (this->enableBits & (1u << index)) != 0;
        }

        AttributeStep getBufferStep(uint32_t index) const
        {
            return (this->instanceBits & (1u << index)) != 0 ? STEP_PER_INSTANCE : STEP_PER_VERTEX;
        }

        void setCommonFormat(CommonFormat format, uint8_t bufferIndex);

        bool operator==(const VertexAttributes& other) const;
    };

    IndexDataType getIndexDataTypeFromMax(size_t maxValue);

    DataFormat getIndexDataFormat(IndexDataType type);

    size_t getIndexDataSize(IndexDataType type);

    // clang-format off
    STRINGMAP_DECLARE(PrimitiveTypes, PrimitiveType,
        { "triangles", PRIMITIVE_TRIANGLES      },
        { "strip",     PRIMITIVE_TRIANGLE_STRIP },
        { "fan",       PRIMITIVE_TRIANGLE_FAN   },
        { "points",    PRIMITIVE_POINTS         }
    );

    STRINGMAP_DECLARE(CullModes, CullMode,
        { "none",  CULL_NONE  },
        { "front", CULL_FRONT },
        { "back",  CULL_BACK  }
    );

    STRINGMAP_DECLARE(WindingModes, Winding,
        { "cw",  WINDING_CW  },
        { "ccw", WINDING_CCW }
    );

    STRINGMAP_DECLARE(BufferAttributes, BuiltinVertexAttribute,
        { "VertexPosition", ATTRIB_POS      },
        { "VertexTexCoord", ATTRIB_TEXCOORD },
        { "VertexColor",    ATTRIB_COLOR    }
    );

    STRINGMAP_DECLARE(IndexDataTypes, IndexDataType,
        { "uint16", INDEX_UINT16 },
        { "uint32", INDEX_UINT32 }
    );

    STRINGMAP_DECLARE(BufferUsages, BufferDataUsage,
        { "stream",   BUFFERDATAUSAGE_STREAM   },
        { "dynamic",  BUFFERDATAUSAGE_DYNAMIC  },
        { "static",   BUFFERDATAUSAGE_STATIC   },
        { "readback", BUFFERDATAUSAGE_READBACK }
    );
    // clang-format on
} // namespace love
