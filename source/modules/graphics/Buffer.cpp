#include "common/Exception.hpp"

#include "common/memory.hpp"
#include "modules/graphics/Buffer.tcc"
#include "modules/graphics/Graphics.tcc"

namespace love
{
    Type BufferBase::type("GraphicsBuffer", &Object::type);

    int BufferBase::bufferCount             = 0;
    int64_t BufferBase::totalGraphicsMemory = 0;

    BufferBase::BufferBase(GraphicsBase* graphics, const Settings& settings, const BufferFormat& bufferFormat,
                           size_t size, size_t length) :
        arrayLength(0),
        arrayStride(0),
        size(size),
        usage(settings.usage),
        dataUsage(settings.dataUsage),
        debugName(settings.debugName),
        mapped(false),
        mappedType(MAP_WRITE_INVALIDATE),
        immutable(false)
    {
        if (size == 0 && length == 0)
            throw love::Exception("Size or array length must be specified.");

        if (bufferFormat.size() == 0)
            throw love::Exception("Data format must contain values.");

        const auto& capabilities = graphics->getCapabilities();

        bool indexBuffer    = usage & BUFFERUSAGEFLAG_INDEX;
        bool vertexBuffer   = usage & BUFFERUSAGEFLAG_VERTEX;
        bool texelBuffer    = usage & BUFFERUSAGEFLAG_TEXEL;
        bool storageBuffer  = usage & BUFFERUSAGEFLAG_SHADER_STORAGE;
        bool indirectBuffer = usage & BUFFERUSAGEFLAG_INDIRECT_ARGUMENTS;

        if (texelBuffer && !capabilities.features[GraphicsBase::FEATURE_TEXEL_BUFFER])
            throw love::Exception("Texel buffers are not supported on this system.");

        if (storageBuffer && !capabilities.features[GraphicsBase::FEATURE_GLSL4])
            throw love::Exception("Shader Storage buffers are not supported on this system.");

        if (storageBuffer && this->dataUsage == BUFFERDATAUSAGE_STREAM)
            throw love::Exception("Cannot use 'stream' buffers as a shader storage buffer.");

        if (indirectBuffer && !capabilities.features[GraphicsBase::FEATURE_INDIRECT_DRAW])
            throw love::Exception("Indirect argument buffers are not supported on this system.");

        const auto anyBuffer = (indexBuffer || vertexBuffer || texelBuffer || storageBuffer || indexBuffer);

        if (this->dataUsage == BUFFERDATAUSAGE_READBACK && anyBuffer)
            throw love::Exception(E_BUFFER_READBACK_TYPE);

        size_t offset = 0;
        size_t stride = 0;
        size_t align  = 1;

        for (const auto& declaration : bufferFormat)
        {
            DataMember member(declaration);
            DataFormat format          = member.declaration.format;
            const DataFormatInfo& info = member.info;

            if (indexBuffer)
            {
                if (format != DATAFORMAT_UINT16 && format != DATAFORMAT_UINT32)
                    throw love::Exception("Index buffers only support uint16_t and uint32_t data types.");

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
                    throw love::Exception("Matrix types are not supported in vertex buffers.");

                if (info.baseType == DATA_BASETYPE_BOOL)
                    throw love::Exception("Bool types are not supported in vertex buffers.");

                const auto location = declaration.bindingLocation;
                if (location < 0 || location >= (int)VertexAttributes::MAX)
                    throw love::Exception(E_INVALID_VERTEX_BIND_LOCATION, VertexAttributes::MAX);
            }

            // TODO: Texel Buffer

            size_t memberOffset = offset;
            size_t memberSize   = member.info.size;

            // TODO: Storage Buffer

            // TODO: Indirect Buffer

            member.offset = memberOffset;
            member.size   = memberSize;
            offset        = member.offset + member.size;
            this->members.push_back(member);
        }

        stride = alignUp(offset, align);

        if (storageBuffer && (indexBuffer || vertexBuffer || texelBuffer))
        {
            if (stride != offset)
                throw love::Exception(E_SHADER_STORAGE_BYTE_OFFSET, stride, offset);
        }

        if (storageBuffer && stride > SHADER_STORAGE_BUFFER_MAX_SRIDE)
            throw love::Exception(E_SHADER_STORAGE_STRIDE, SHADER_STORAGE_BUFFER_MAX_SRIDE);

        if (size != 0)
        {
            size_t remainder = size % stride;
            if (remainder > 0)
                size += stride - remainder;
            length = size / stride;
        }
        else
            size = length * stride;

        this->arrayStride = stride;
        this->arrayLength = length;
        this->size        = size;

        const auto texelBufferSize = capabilities.limits[GraphicsBase::LIMIT_TEXEL_BUFFER_SIZE];
        if (texelBuffer && length * this->members.size() > texelBufferSize)
            throw love::Exception(E_TEXEL_BUFFER_SIZE, this->members.size(), length, texelBufferSize);

        ++BufferBase::bufferCount;
        BufferBase::totalGraphicsMemory += size;
    }

    BufferBase::~BufferBase()
    {
        BufferBase::totalGraphicsMemory -= this->size;
        --BufferBase::bufferCount;
    }

    int BufferBase::getDataMemberIndex(const std::string& name) const
    {
        for (size_t index = 0; index < this->members.size(); index++)
        {
            if (this->members[index].declaration.name == name)
                return (int)index;
        }

        return -1;
    }

    int BufferBase::getDataMemberIndex(int location) const
    {
        for (size_t index = 0; index < this->members.size(); index++)
        {
            if (this->members[index].declaration.bindingLocation == location)
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
            throw love::Exception("Offset and size parameters to clear() are not within the Buffer's size.");
        else if (offset % 4 != 0 || size % 4 != 0)
            throw love::Exception(E_BUFFER_CLEAR_NOT_MULTIPLES_OF_4);

        this->clearInternal(offset, size);
    }

    std::vector<BufferBase::DataDeclaration> BufferBase::getCommonFormatDeclaration(CommonFormat format)
    {
        switch (format)
        {
            case CommonFormat::NONE:
                return {};
            case CommonFormat::XYf:
                return { { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_POS } };
            case CommonFormat::XYZf:
                return { { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC3, 0, ATTRIB_POS } };
            case CommonFormat::RGBAf:
                return { { getConstant(ATTRIB_COLOR), DATAFORMAT_FLOAT_VEC4, 0, ATTRIB_COLOR } };
            case CommonFormat::STf_RGBAf:
                return { { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_TEXCOORD },
                         { getConstant(ATTRIB_COLOR), DATAFORMAT_FLOAT_VEC4, 0, ATTRIB_COLOR } };
            case CommonFormat::XYf_STf:
                return { { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_POS },
                         { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_TEXCOORD } };
            case CommonFormat::XYf_STPf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_POS },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC3, 0, ATTRIB_TEXCOORD },
                };
            case CommonFormat::XYf_STf_RGBAf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_POS },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_TEXCOORD },
                    { getConstant(ATTRIB_COLOR), DATAFORMAT_FLOAT_VEC4, 0, ATTRIB_COLOR },
                };
            case CommonFormat::XYf_STus_RGBAf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_POS },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_UNORM16_VEC2, 0, ATTRIB_TEXCOORD },
                    { getConstant(ATTRIB_COLOR), DATAFORMAT_FLOAT_VEC4, 0, ATTRIB_COLOR },
                };
            case CommonFormat::XYf_STPf_RGBAf:
                return {
                    { getConstant(ATTRIB_POS), DATAFORMAT_FLOAT_VEC2, 0, ATTRIB_POS },
                    { getConstant(ATTRIB_TEXCOORD), DATAFORMAT_FLOAT_VEC3, 0, ATTRIB_TEXCOORD },
                    { getConstant(ATTRIB_COLOR), DATAFORMAT_FLOAT_VEC4, 0, ATTRIB_COLOR },
                };
            case CommonFormat::COUNT:
                return {};
        }

        return {};
    }
} // namespace love
