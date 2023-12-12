#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/resource.hpp>
#include <common/vector.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <cstring>

namespace love
{
    namespace vertex
    {
        struct Vertex
        {
            Vector3 position;
            std::array<float, 4> color;
            std::array<float, 2> texcoord;
        };

        static constexpr size_t POSITION_OFFSET = offsetof(Vertex, position);
        static constexpr size_t COLOR_OFFSET    = offsetof(Vertex, color);
        static constexpr size_t TEXCOORD_OFFSET = offsetof(Vertex, texcoord);

        struct STf_RGBAf
        {
            float s, t;
            Color color;
        };

        enum IndexDataType
        {
            INDEX_UINT16,
            INDEX_UINT32,
            INDEX_MAX_ENUM
        };

        enum CullMode
        {
            CULL_NONE,
            CULL_BACK,
            CULL_FRONT,
            CULL_MAX_ENUM
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

        enum PrimitiveType
        {
            PRIMITIVE_TRIANGLES,
            PRIMITIVE_TRIANGLE_STRIP,
            PRIMITIVE_TRIANGLE_FAN,
            PRIMITIVE_QUADS,
            PRIMITIVE_POINTS,
            PRIMITIVE_MAX_ENUM
        };

        /*
        ** Position, Color
        ** Position, TexCoords, Color
        */
        enum class CommonFormat
        {
            NONE,
            PRIMITIVE,
            TEXTURE,
            FONT,
            XYf,
            XYZf,
            STf_RGBAf
        };

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

        enum AttributeStep
        {
            STEP_PER_VERTEX,
            STEP_PER_INSTANCE,
            STEP_MAX_ENUM
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

        // The expected usage pattern of buffer data.
        enum BufferDataUsage
        {
            BUFFERDATAUSAGE_STREAM,
            BUFFERDATAUSAGE_DYNAMIC,
            BUFFERDATAUSAGE_STATIC,
            BUFFERDATAUSAGE_READBACK,
            BUFFERDATAUSAGE_MAX_ENUM
        };

        struct BufferBindings
        {
            static constexpr uint32_t MAX = 32;

            uint32_t useBits = 0;

            struct
            {
                Resource* buffer;
                size_t offset;
            } info[MAX];

            void Set(uint32_t index, Resource* buffer, size_t offset)
            {
                this->useBits |= (1u << index);
                this->info[index].buffer = buffer;
                this->info[index].offset = offset;
            }

            void Disable(uint32_t index)
            {
                this->useBits &= ~(1u << index);
            }

            void Clear()
            {
                this->useBits = 0;
            }
        };

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
            ATTRIBFLAG_COLOR    = 1 << ATTRIB_COLOR,
        };

        struct VertexAttributeInfo
        {
            uint8_t bufferIndex;
            DataFormat format : 8;
            uint16_t offset;
        };

        struct VertexBufferLayout
        {
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
                SetCommonFormat(format, bufferIndex);
            }

            void Set(uint32_t index, DataFormat format, uint16_t offsetFromVertex,
                     uint8_t bufferIndex)
            {
                this->enableBits |= (1u << index);
                this->attributes[index] = { bufferIndex, format, offsetFromVertex };
            }

            void SetBufferLayout(uint8_t bufferIndex, uint16_t stride, AttributeStep step)
            {
                uint32_t bufferBit = (1u << bufferIndex);

                if (step == STEP_PER_INSTANCE)
                    this->instanceBits |= bufferBit;
                else
                    this->instanceBits &= ~bufferBit;

                this->bufferLayouts[bufferIndex].stride = stride;
            }

            void Disable(uint32_t index)
            {
                this->enableBits &= ~(1u << index);
            }

            void Clear()
            {
                this->enableBits = 0;
            }

            bool IsEnabled(uint32_t index) const
            {
                return (this->enableBits & (1u << index)) != 0;
            }

            AttributeStep GetBufferStep(uint32_t index) const
            {
                const auto hasInstance = (this->instanceBits & (1u << index)) != 0;
                return hasInstance ? STEP_PER_INSTANCE : STEP_PER_VERTEX;
            }

            void SetCommonFormat(CommonFormat format, uint8_t bufferIndex)
            {
                switch (format)
                {
                    case CommonFormat::NONE:
                        break;
                    case CommonFormat::PRIMITIVE:
                    {
                        Set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC3, 0, bufferIndex);
                        Set(ATTRIB_COLOR, DATAFORMAT_FLOAT_VEC4, COLOR_OFFSET, bufferIndex);
                        break;
                    }
                    case CommonFormat::TEXTURE:
                    case CommonFormat::FONT:
                    {
                        Set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC3, 0, bufferIndex);
                        Set(ATTRIB_COLOR, DATAFORMAT_FLOAT_VEC4, COLOR_OFFSET, bufferIndex);
                        Set(ATTRIB_TEXCOORD, DATAFORMAT_FLOAT_VEC2, TEXCOORD_OFFSET, bufferIndex);
                        break;
                    }
                    case CommonFormat::XYf:
                    {
                        Set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC2, 0, bufferIndex);
                        break;
                    }
                    case CommonFormat::XYZf:
                    {
                        Set(ATTRIB_POS, DATAFORMAT_FLOAT_VEC3, 0, bufferIndex);
                        break;
                    }
                    case CommonFormat::STf_RGBAf:
                    {
                        Set(ATTRIB_TEXCOORD, DATAFORMAT_FLOAT_VEC2, TEXCOORD_OFFSET, bufferIndex);
                        Set(ATTRIB_COLOR, DATAFORMAT_FLOAT_VEC4, COLOR_OFFSET, bufferIndex);
                        break;
                    }
                }
            }

            bool operator==(const VertexAttributes& other) const
            {
                if (this->enableBits != other.enableBits ||
                    this->instanceBits != other.instanceBits)
                {
                    return false;
                }

                uint32_t allBits = this->enableBits;
                uint32_t index   = 0;

                while (allBits)
                {
                    if (this->IsEnabled(index))
                    {
                        const auto& thisAttrib  = this->attributes[index];
                        const auto& otherAttrib = other.attributes[index];

                        if (thisAttrib.bufferIndex != otherAttrib.bufferIndex ||
                            thisAttrib.format != otherAttrib.format ||
                            thisAttrib.offset != otherAttrib.offset)
                        {
                            return false;
                        }

                        if (this->bufferLayouts[thisAttrib.bufferIndex].stride !=
                            other.bufferLayouts[thisAttrib.bufferIndex].stride)
                        {
                            return false;
                        }
                    }

                    index++;
                    allBits >>= 1;
                }

                return true;
            }
        };

        void FillIndices(TriangleIndexMode mode, uint16_t start, uint16_t count, uint16_t* indices);

        void FillIndices(TriangleIndexMode mode, uint32_t start, uint32_t count, uint32_t* indices);

        static inline int GetIndexCount(TriangleIndexMode mode, int vertexCount)
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

        static inline size_t GetFormatStride(CommonFormat format)
        {
            switch (format)
            {
                case CommonFormat::NONE:
                    return 0;
                case CommonFormat::XYf:
                    return sizeof(float) * 2;
                case CommonFormat::XYZf:
                    return sizeof(float) * 3;
                case CommonFormat::STf_RGBAf:
                    return sizeof(STf_RGBAf);
                default:
                    return 0;
            }

            return 0;
        }

        static inline CommonFormat GetSinglePositionFormat(bool is2D)
        {
            /* the 3DS GPU expects us to upload all 3 vertices */
            if (is2D && Console::Is(Console::CTR))
                return CommonFormat::XYZf;

            return is2D ? CommonFormat::XYf : CommonFormat::XYZf;
        }

        const DataFormatInfo& GetDataFormatInfo(DataFormat format);

        static constexpr size_t VERTEX_SIZE = sizeof(Vertex);

        // clang-format off
        static constexpr BidirectionalMap cullModes = {
            "none",  CULL_NONE,
            "back",  CULL_BACK,
            "front", CULL_FRONT
        };

        static constexpr BidirectionalMap windingModes = {
            "cw",  WINDING_CW,
            "ccw", WINDING_CCW
        };

        static constexpr BidirectionalMap triangleModes = {
            "none",  TRIANGLEINDEX_NONE,
            "strip", TRIANGLEINDEX_STRIP,
            "fan",   TRIANGLEINDEX_FAN,
            "quads", TRIANGLEINDEX_QUADS
        };

        static constexpr BidirectionalMap primitiveTypes = {
            "triangles",  PRIMITIVE_TRIANGLES,
            "strip",      PRIMITIVE_TRIANGLE_STRIP,
            "fan",        PRIMITIVE_TRIANGLE_FAN,
            "points",     PRIMITIVE_POINTS
        };
        // clang-format on
    } // namespace vertex
} // namespace love
