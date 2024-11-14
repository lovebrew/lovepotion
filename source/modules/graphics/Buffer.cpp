#include "common/memory.hpp"

#include "modules/graphics/Buffer.tcc"
#include "modules/graphics/Graphics.tcc"

#include "modules/graphics/vertex.hpp"

namespace love
{
    BufferBase::BufferBase(GraphicsBase* graphics, const Settings& settings,
                           const std::vector<DataDeclaration>& bufferFormat, size_t size, size_t length) :
        arrayLength(0),
        arrayStride(0),
        size(size),
        usageFlags(settings.usageFlags),
        dataUsage(settings.dataUsage),
        debugName(settings.debugName),
        mapped(false),
        mappedType(MAP_WRITE_INVALIDATE),
        immutable(false)
    {
        if (size == 0 && arrayLength == 0)
            throw love::Exception("Size or array length must be specified.");

        if (bufferFormat.size() == 0)
            throw love::Exception("Data format must contain values.");

        const auto& caps = graphics->getCapabilities();

        bool indexBuffer  = usageFlags & BUFFERUSAGEFLAG_INDEX;
        bool vertexBuffer = usageFlags & BUFFERUSAGEFLAG_VERTEX;

        size_t offset             = 0;
        size_t stride             = 0;
        size_t structureAlignment = 1;

        for (const DataDeclaration& declaration : bufferFormat)
        {
            DataMember member(declaration);

            DataFormat format          = member.decl.format;
            const DataFormatInfo& info = member.info;

            if (indexBuffer)
            {
                if (!caps.features[GraphicsBase::FEATURE_INDEX_BUFFER_32BIT] && format == DATAFORMAT_UINT32)
                    throw love::Exception("32-bit index buffers are not supported by this system.");

                if (format != DATAFORMAT_UINT16 && format != DATAFORMAT_UINT32)
                    throw love::Exception("Index buffers only support uint16 and uint32 data types.");

                if (bufferFormat.size() > 1)
                    throw love::Exception("Index buffers only support a single value per element.");

                if (declaration.arrayLength > 0)
                    throw love::Exception("Arrays are not supported in index buffers.");
            }

            if (vertexBuffer)
            {
                if (declaration.arrayLength > 0)
                    throw love::Exception("Arrays are not supported in vertex buffers.");

                if (info.isMatrix)
                    throw love::Exception("Matrix data types are not supported in vertex buffers.");

                if (info.baseType == DATA_BASETYPE_BOOL)
                    throw love::Exception("Boolean data types are not supported in vertex buffers.");

                if (declaration.name.empty())
                    throw love::Exception("Vertex buffer data members must have a name.");
            }

            member.offset = offset;
            member.size   = member.info.size;

            offset = member.offset + member.size;
            this->dataMembers.push_back(member);
        }

        // stride = alignUp(offset, structureAlignment);

        if (size != 0)
        {
            size_t remainder = size % stride;

            if (remainder > 0)
                size += stride - remainder;

            arrayLength = size / stride;
        }
        else
            size = arrayLength * stride;

        this->arrayStride = stride;
        this->arrayLength = arrayLength;
        this->size        = size;

        ++bufferCount;
        totalGraphicsMemory += size;
    }

    BufferBase::~BufferBase()
    {
        totalGraphicsMemory -= size;
        --bufferCount;
    }

    int BufferBase::getDataMemberIndex(const std::string& name) const
    {
        for (size_t index = 0; index < this->dataMembers.size(); index++)
        {
            if (this->dataMembers[index].decl.name == name)
                return (int)index;
        }

        return -1;
    }

    void BufferBase::clear(size_t offset, size_t size)
    {
        if (this->isImmutable())
            throw love::Exception("Cannot clear an immutable Buffer.");
        else if (this->isMapped())
            throw love::Exception("Cannot clear a mapped Buffer.");
        else if (offset + size > this->getSize())
            throw love::Exception(
                "The given offset and size parameters to clear() are not within the Buffer's size.");
        else if (offset % 4 != 0 || size % 4 != 0)
            throw love::Exception(
                "clear() must be used with offset and size parameters that are multiples of 4 bytes.");

        this->clearInternal(offset, size);
    }

    std::vector<BufferBase::DataDeclaration> BufferBase::getCommonFormatDeclaration(CommonFormat format)
    {
        switch (format)
        {
            case CommonFormat::NONE:
                return {};
            case CommonFormat::XYf:
                return { { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2 } };
            case CommonFormat::XYZf:
                return { { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC3 } };
            case CommonFormat::RGBAf:
                return { { getConstant(ATTRIB_COLOR), DATAFORMAT_UNORM8_VEC4 } };
            case CommonFormat::STf_RGBAf:
                return {
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC2 },
                    { getConstant(ATTRIB_COLOR), DATAFORMAT_UNORM8_VEC4 },
                };
            case CommonFormat::STPf_RGBAf:
                return {
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC3 },
                    { getConstant(ATTRIB_COLOR), DATAFORMAT_UNORM8_VEC4 },
                };
            case CommonFormat::XYf_STf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2 },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC2 },
                };
            case CommonFormat::XYf_STPf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2 },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC3 },
                };
            case CommonFormat::XYf_STf_RGBAf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2 },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC2 },
                    { getConstant(ATTRIB_COLOR), DATAFORMAT_UNORM8_VEC4 },
                };
            case CommonFormat::XYf_STus_RGBAf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2 },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_UNORM16_VEC2 },
                    { getConstant(ATTRIB_COLOR), DATAFORMAT_UNORM8_VEC4 },
                };
            case CommonFormat::XYf_STPf_RGBAf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2 },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC2 },
                    { getConstant(ATTRIB_COLOR), DATAFORMAT_UNORM8_VEC4 },
                };
            default:
                break;
        }

        return {};
    }
} // namespace love
