#include "common/Exception.hpp"
#include "common/Matrix.hpp"

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Mesh.hpp"

#include <algorithm>
#include <limits>

#include "common/debug.hpp"

#define E_MESH_HAS_NO_ATTRIBUTE_NAME "Mesh does not have an attached vertex attribute named '{:s}'"
#define E_MESH_HAS_NO_ATTRIBUTE_INDEX \
    "Mesh does not have an attached vertex attribute with binding location {:d}'"

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

        // clang-format off
        BufferBase::Settings settings(BUFFERUSAGEFLAG_VERTEX, usage);
        this->vertexBuffer.set(graphics->newBuffer(settings, vertexFormat, vertexData, dataSize, 0), Acquire::NO_RETAIN);
        // clang-format on

        this->vertexCount  = this->vertexBuffer->getArrayLength();
        this->vertexStride = this->vertexBuffer->getArrayStride();
        this->vertexFormat = this->vertexBuffer->getDataMembers();

        this->setupAttachedAttributes();
        this->indexType = getIndexDataTypeFromMax(vertexCount);
    }

    Mesh::Mesh(GraphicsBase* graphics, const std::vector<BufferBase::DataDeclaration>& vertexformat,
               int vertexcount, PrimitiveType drawmode, BufferDataUsage usage) :
        vertexCount((size_t)vertexcount),
        indexType(getIndexDataTypeFromMax(vertexcount)),
        drawMode(drawmode)
    {
        if (vertexcount <= 0)
            throw love::Exception("Invalid number of vertices ({:d}).", vertexcount);

        // clang-format off
        BufferBase::Settings settings(BUFFERUSAGEFLAG_VERTEX, usage);
        this->vertexBuffer.set(graphics->newBuffer(settings, vertexformat, nullptr, 0, vertexcount), Acquire::NO_RETAIN);
        // clang-format on

        this->vertexStride = this->vertexBuffer->getArrayStride();
        this->vertexFormat = this->vertexBuffer->getDataMembers();

        this->setupAttachedAttributes();

        try
        {
            this->vertexData = new uint8_t[this->vertexBuffer->getSize()];
        }
        catch (std::exception&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        std::memset(this->vertexData, 0, this->vertexBuffer->getSize());
        this->vertexBuffer->fill(0, this->vertexBuffer->getSize(), this->vertexData);
    }

    Mesh::Mesh(const std::vector<Mesh::BufferAttribute>& attributes, PrimitiveType drawmode) :
        drawMode(drawmode)
    {}

    Mesh::~Mesh()
    {
        delete this->vertexData;
        if (this->indexData != nullptr)
            std::free(this->indexData);
    }

    int Mesh::getAttachedAttributeIndex(const std::string& name) const
    {
        for (int index = 0; index < (int)this->attachedAttributes.size(); index++)
        {
            if (this->attachedAttributes[index].name == name)
                return index;
        }

        return -1;
    }

    int Mesh::getAttachedAttributeIndex(int location) const
    {
        for (int index = 0; index < (int)this->attachedAttributes.size(); index++)
        {
            if (this->attachedAttributes[index].bindingLocation == location)
                return index;
        }

        return -1;
    }

    void Mesh::finalizeAttribute(BufferAttribute& attribute) const
    {}

    void* Mesh::checkVertexDataOffset(size_t index, size_t* byteOffset)
    {
        if (index >= this->vertexCount)
            throw love::Exception("Invalid vertex index: {:ld}", index + 1);

        if (this->vertexData == nullptr)
            throw love::Exception("Mesh must own its own vertex buffer.");

        size_t offset = index * this->vertexStride;

        if (byteOffset != nullptr)
            *byteOffset = offset;

        return this->vertexData + offset;
    }

    size_t Mesh::getVertexCount() const
    {
        return this->vertexCount;
    }

    size_t Mesh::getVertexStride() const
    {
        return this->vertexStride;
    }

    BufferBase* Mesh::getVertexBuffer() const
    {
        return this->vertexBuffer;
    }

    const std::vector<BufferBase::DataMember>& Mesh::getVertexFormat() const
    {
        return this->vertexFormat;
    }

    void Mesh::setAttributeEnabled(const std::string& name, bool enable)
    {
        int index = this->getAttachedAttributeIndex(name);
        if (index == -1)
            throw love::Exception(E_MESH_HAS_NO_ATTRIBUTE_NAME, name);

        this->attachedAttributes[index].enabled = enable;
        this->attributesID.invalidate();
    }

    bool Mesh::isAttributeEnabled(const std::string& name) const
    {
        int index = this->getAttachedAttributeIndex(name);
        if (index == -1)
            throw love::Exception(E_MESH_HAS_NO_ATTRIBUTE_NAME, name);

        return this->attachedAttributes[index].enabled;
    }

    void Mesh::setAttributeEnabled(int location, bool enable)
    {
        int index = this->getAttachedAttributeIndex(location);
        if (index == -1)
            throw love::Exception(E_MESH_HAS_NO_ATTRIBUTE_INDEX, location);

        this->attachedAttributes[index].enabled = enable;
        this->attributesID.invalidate();
    }

    bool Mesh::isAttributeEnabled(int location) const
    {
        int index = this->getAttachedAttributeIndex(location);
        if (index == -1)
            throw love::Exception(E_MESH_HAS_NO_ATTRIBUTE_INDEX, location);

        return this->attachedAttributes[index].enabled;
    }

    void Mesh::attachAttribute(const std::string& name, BufferBase* buffer, Mesh* mesh,
                               const std::string& attachName, int startIndex, AttributeStep step)
    {}

    void Mesh::attachAttribute(int bindingLocation, BufferBase* buffer, Mesh* mesh, int attachBindingLocation,
                               int startindex, AttributeStep step)
    {}

    bool Mesh::detachAttribute(const std::string& name)
    {
        int index = this->getAttachedAttributeIndex(name);
        if (index == -1)
            return false;

        // TODO: something

        return true;
    }

    bool Mesh::detachAttribute(int location)
    {
        int index = this->getAttachedAttributeIndex(location);
        if (index == -1)
            return false;

        // TODO: something

        return true;
    }

    const std::vector<Mesh::BufferAttribute>& Mesh::getAttachedAttributes() const
    {
        return this->attachedAttributes;
    }

    void* Mesh::getVertexData() const
    {
        return this->vertexData;
    }

    void Mesh::setVertexDataModified(size_t offset, size_t size)
    {
        if (this->vertexData != nullptr)
            this->modifiedVertexData.encapsulate(offset, size);
    }

    void Mesh::flush()
    {
        if (this->vertexBuffer.get() && this->vertexData != nullptr && this->modifiedVertexData.isValid())
        {
            if (this->vertexBuffer->getDataUsage() == BUFFERDATAUSAGE_STREAM)
                this->vertexBuffer->fill(0, this->vertexBuffer->getSize(), this->vertexData);
            else
            {
                size_t offset = this->modifiedVertexData.getOffset();
                size_t size   = this->modifiedVertexData.getSize();
                this->vertexBuffer->fill(offset, size, this->vertexData + offset);
            }
        }

        if (this->indexDataModified && this->indexData != nullptr && this->indexBuffer != nullptr)
        {
            this->indexBuffer->fill(0, this->indexBuffer->getSize(), this->indexData);
            this->indexDataModified = false;
        }
    }

    /**
     * Copies index data from a vector to a mapped index buffer.
     **/
    template<typename T>
    static void copyToIndexBuffer(const std::vector<uint32_t>& indices, void* data, size_t maxval)
    {
        T* elems = (T*)data;

        for (size_t i = 0; i < indices.size(); i++)
        {
            if (indices[i] >= maxval)
                throw love::Exception("Invalid vertex map value: %d", indices[i] + 1);

            elems[i] = (T)indices[i];
        }
    }

    void Mesh::setVertexMap(const std::vector<uint32_t>& map)
    {
        if (map.empty())
            throw love::Exception("Vertex map array must not be empty.");

        size_t maxValue = this->getVertexCount();
        auto type       = getIndexDataTypeFromMax(maxValue);
        auto format     = getIndexDataFormat(type);

        size_t size = map.size() * getIndexDataSize(type);

        bool recreate = this->indexData == nullptr || this->indexBuffer.get() == nullptr ||
                        size > this->indexBuffer->getSize() ||
                        this->indexBuffer->getDataMember(0).declaration.format != format;

        if (recreate)
        {
            auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);
            auto usage =
                this->vertexBuffer.get() ? this->vertexBuffer->getDataUsage() : BUFFERDATAUSAGE_DYNAMIC;
            BufferBase::Settings settings(BUFFERUSAGE_INDEX, usage);
            auto buffer = StrongRef<BufferBase>(graphics->newBuffer(settings, format, nullptr, size, 0),
                                                Acquire::NO_RETAIN);

            auto* data = (uint8_t*)std::realloc(this->indexData, size);
            if (data == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);

            this->indexData   = data;
            this->indexBuffer = buffer;
        }

        this->indexCount     = map.size();
        this->useIndexBuffer = true;
        this->indexType      = type;

        if (this->indexCount == 0)
            return;

        switch (type)
        {
            case INDEX_UINT16:
                copyToIndexBuffer<uint16_t>(map, this->indexData, maxValue);
                break;
            case INDEX_UINT32:
                copyToIndexBuffer<uint32_t>(map, this->indexData, maxValue);
                break;
        }

        this->indexDataModified = true;
    }

    void Mesh::setVertexMap(IndexDataType datatype, const void* data, size_t datasize)
    {}

    void Mesh::setVertexMap()
    {
        this->useIndexBuffer = false;
    }

    /**
     * Copies index data from a mapped buffer to a vector.
     **/
    template<typename T>
    static void copyFromIndexBuffer(const void* buffer, size_t count, std::vector<uint32_t>& indices)
    {
        const T* elems = (const T*)buffer;
        for (size_t i = 0; i < count; i++)
            indices.push_back((uint32_t)elems[i]);
    }

    bool Mesh::getVertexMap(std::vector<uint32_t>& map) const
    {
        if (!this->useIndexBuffer)
            return false;

        map.clear();

        if (this->indexData == nullptr || this->indexCount == 0)
            return true;

        map.reserve(this->indexCount);

        switch (this->indexType)
        {
            case INDEX_UINT16:
                copyFromIndexBuffer<uint16_t>(this->indexData, this->indexCount, map);
                break;
            case INDEX_UINT32:
            default:
                copyFromIndexBuffer<uint32_t>(this->indexData, this->indexCount, map);
                break;
        }

        return true;
    }

    void Mesh::setIndexBuffer(BufferBase* buffer)
    {
        if (buffer != nullptr && (buffer->getUsageFlags() & BUFFERUSAGEFLAG_INDEX) == 0)
            throw love::Exception("setIndexBuffer requires a Buffer created as an index buffer");

        this->indexBuffer.set(buffer);
        this->useIndexBuffer = buffer != nullptr;
        this->indexCount     = buffer != nullptr ? buffer->getArrayLength() : 0;

        if (buffer != nullptr)
            this->indexType = getIndexDataType(buffer->getDataMember(0).declaration.format);

        if (this->indexData != nullptr)
        {
            std::free(this->indexData);
            this->indexData = nullptr;
        }
    }

    BufferBase* Mesh::getIndexBuffer() const
    {
        return this->indexBuffer;
    }

    size_t Mesh::getIndexCount() const
    {
        return this->indexCount;
    }

    void Mesh::setTexture(TextureBase* texture)
    {
        this->texture.set(texture);
    }

    void Mesh::setTexture()
    {
        this->texture.set(nullptr);
    }

    TextureBase* Mesh::getTexture() const
    {
        return this->texture.get();
    }

    void Mesh::setDrawMode(PrimitiveType mode)
    {
        this->drawMode = mode;
    }

    PrimitiveType Mesh::getDrawMode() const
    {
        return this->drawMode;
    }

    void Mesh::setDrawRange(int start, int count)
    {
        if (start < 0 || count <= 0)
            throw love::Exception("Invalid draw range.");

        this->drawRange = Range(start, count);
    }

    void Mesh::setDrawRange()
    {
        this->drawRange.invalidate();
    }

    bool Mesh::getDrawRange(int& start, int& count) const
    {
        if (!this->drawRange.isValid())
            return false;

        start = (int)this->drawRange.getOffset();
        count = (int)this->drawRange.getSize();
        return true;
    }

    void Mesh::updateVertexAttributes(GraphicsBase* graphics)
    {
        VertexAttributes attributes {};
        BufferBindings& buffers = this->bufferBindings;
        int activeBuffers       = 0;

        for (const auto& attribute : this->attachedAttributes)
        {
            if (!attribute.enabled)
                continue;

            auto* buffer = attribute.buffer.get();
            int location = attribute.bindingLocation;

            // if (location < 0 && ShaderBase::current)
            //     continue;

            if (location >= 0)
            {
                const auto& member  = buffer->getDataMember(attribute.indexInBuffer);
                uint16_t offset     = (uint16_t)member.offset;
                uint16_t stride     = (uint16_t)buffer->getArrayStride();
                size_t bufferOffset = (size_t)attribute.startArrayIndex * stride;

                int bufferIndex = activeBuffers;
                for (int i = 0; i < activeBuffers; i++)
                {
                    if (buffers.info[i].buffer == buffer && buffers.info[i].offset == bufferOffset &&
                        attributes.bufferLayouts[i].stride == stride &&
                        attributes.getBufferStep(i) == attribute.step)
                    {
                        bufferIndex = i;
                        break;
                    }
                }

                attributes.set(location, member.declaration.format, offset, bufferIndex);
                attributes.setBufferLayout(bufferIndex, stride, attribute.step);
                buffers.set(bufferIndex, buffer, bufferOffset);
                activeBuffers = std::max(activeBuffers, bufferIndex + 1);
            }
        }

        this->attributesID = graphics->registerVertexAttributes(attributes);
    }

    void Mesh::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        this->drawInternal(graphics, matrix, 1, nullptr, 0);
    }

    void Mesh::drawInternal(GraphicsBase* graphics, const Matrix4& matrix, int instanceCount,
                            BufferBase* indirectArgs, int argsIndex)
    {
        if (this->vertexCount <= 0 || (instanceCount <= 0 && indirectArgs == nullptr))
            return;

        if (indirectArgs != nullptr)
        {
            if (this->drawMode == PRIMITIVE_TRIANGLE_FAN)
                throw love::Exception("The fan draw mode is not supported in indirect draws.");
        }

        graphics->flushBatchedDraws();
        this->flush();

        bool attributesIDNeedsUpdate = !this->attributesID.isValid();
        for (const auto& attribute : this->attachedAttributes)
        {
            if (!attribute.enabled)
                continue;

            if (attribute.mesh.get())
                attribute.mesh->flush();

            if (attribute.bindingLocation < 0)
                attributesIDNeedsUpdate = true;
        }

        if (attributesIDNeedsUpdate)
            this->updateVertexAttributes(graphics);

        GraphicsBase::TempTransform transform(graphics, matrix);

        BufferBase* indexBuffer = this->useIndexBuffer ? this->indexBuffer : nullptr;
        int indexCount          = (int)this->indexCount;
        Range range             = this->drawRange;

        if (this->drawMode == PRIMITIVE_TRIANGLE_FAN && indexBuffer == nullptr)
        {
            indexCount = love::getIndexCount(TRIANGLEINDEX_FAN, this->vertexCount);
            if (range.isValid())
            {
                int start = love::getIndexCount(TRIANGLEINDEX_FAN, (int)range.getOffset());
                int size  = love::getIndexCount(TRIANGLEINDEX_FAN, (int)range.getSize());
                range     = Range(start, size);
            }
        }

        if (indexBuffer != nullptr && (this->indexCount > 0 || indirectArgs != nullptr))
        {
            Range r(0, indexCount);
            if (range.isValid())
                r.intersect(range);

            DrawIndexedCommand command(this->attributesID, &this->bufferBindings, indexBuffer);
            command.primitiveType     = this->drawMode;
            command.indexType         = this->indexType;
            command.instanceCount     = instanceCount;
            command.texture           = this->texture;
            command.cullMode          = graphics->getMeshCullMode();
            command.indexBufferOffset = r.getOffset() * indexBuffer->getArrayStride();
            command.indexCount        = r.getSize();

            if (command.indexCount > 0)
                graphics->draw(command);
        }
        else if (this->vertexCount > 0 || indirectArgs != nullptr)
        {
            Range r(0, this->vertexCount);
            if (range.isValid())
                r.intersect(range);

            DrawCommand command(this->attributesID, &this->bufferBindings);
            command.primitiveType = this->drawMode;
            command.vertexStart   = r.getOffset();
            command.vertexCount   = r.getSize();
            command.instanceCount = instanceCount;
            command.texture       = this->texture;
            command.cullMode      = graphics->getMeshCullMode();

            if (command.vertexCount > 0)
                graphics->draw(command);
        }
    }

    void Mesh::setupAttachedAttributes()
    {
        for (size_t i = 0; i < this->vertexFormat.size(); i++)
        {
            const auto& name = this->vertexFormat[i].declaration.name;
            auto location    = this->vertexFormat[i].declaration.bindingLocation;

            if (location >= 0)
            {
                if (this->getAttachedAttributeIndex(location) != -1)
                    throw love::Exception("Duplicate vertex attribute binding location: {:d}.", location);
            }

            if (!name.empty())
            {
                if (this->getAttachedAttributeIndex(name) != -1)
                    throw love::Exception("Duplicate vertex attribute name: '{:s}'.", name);
            }

            if (location < 0)
            {
                BuiltinVertexAttribute attribute {};
                if (getConstant(name.c_str(), attribute))
                    location = (int)attribute;
            }

            // clang-format off
            this->attachedAttributes.push_back({ name, this->vertexBuffer, nullptr, name, location, (int)i, 0, STEP_PER_VERTEX, location, true });
            // clang-format on
        }
    }
} // namespace love
