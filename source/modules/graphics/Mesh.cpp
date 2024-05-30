#include "common/Exception.hpp"
#include "common/Matrix.hpp"

#include "modules/graphics/Mesh.hpp"

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Shader.tcc"

#include <algorithm>
#include <limits>

namespace love
{
    // clang-format off
    static_assert(offsetof(Vertex, x) == sizeof(float) * 0, "Incorrect position offset in Vertex struct");
    static_assert(offsetof(Vertex, s) == sizeof(float) * 2, "Incorrect texture coordinate offset in Vertex struct");
    static_assert(offsetof(Vertex, color.r) == sizeof(float) * 4, "Incorrect color offset in Vertex struct");
    // clang-format on

    Type Mesh::type("Mesh", &Drawable::type);

    Mesh::Mesh(GraphicsBase* graphics, const void* data, size_t size, PrimitiveType mode,
               BufferDataUsage usage) :
        primitiveType(mode)
    {
        if (size == 0)
            throw love::Exception("Mesh data size cannot be zero.");

        size_t vertexCount = size / sizeof(Vertex);
        this->buffer.resize(vertexCount);

        this->indexDataType = getIndexDataTypeFromMax(vertexCount);
    }

    Mesh::Mesh(GraphicsBase* graphics, int vertexCount, PrimitiveType mode, BufferDataUsage usage) :
        primitiveType(mode)
    {
        if (vertexCount <= 0)
            throw love::Exception("Mesh vertex count cannot be zero.");

        this->buffer.resize(vertexCount);
    }

    Mesh::~Mesh()
    {}

    void* Mesh::checkVertexDataOffset(size_t vertexIndex, size_t* byteOffset)
    {
        if (vertexIndex >= this->vertexCount)
            throw love::Exception("Invalid vertex index: {:d}", vertexIndex + 1);

        if (this->buffer.empty())
            throw love::Exception("Mesh must own its own vertex buffer.");

        size_t offset = vertexIndex * sizeof(Vertex);

        if (byteOffset != nullptr)
            *byteOffset = offset;

        return this->buffer.data() + offset;
    }

    size_t Mesh::getVertexCount() const
    {
        return this->vertexCount;
    }

    size_t Mesh::getVertexStride() const
    {
        return sizeof(Vertex);
    }

    Vertex* Mesh::getVertexBuffer()
    {
        return this->buffer.data();
    }

    void* Mesh::getVertexData()
    {
        return (void*)this->buffer.data();
    }

    void Mesh::setVertexDataModified(size_t offset, size_t size)
    {
        if (!this->buffer.empty())
            this->modifiedVertexData.encapsulate(offset, size);
    }

    void Mesh::flush()
    {
        if (this->buffer.data() != nullptr && this->modifiedVertexData.isValid())
        {
            size_t offset = this->modifiedVertexData.getOffset();
            size_t size   = this->modifiedVertexData.getSize();

            this->modifiedVertexData.invalidate();
        }
    }

    template<typename T>
    static void copyToIndexBuffer(const std::vector<uint32_t>& indices, void* data, size_t maxValue)
    {
        T* elements = (T*)data;

        for (size_t index = 0; index < indices.size(); index++)
        {
            if (indices[index] > maxValue)
                throw love::Exception("Invalid vertex map value: {:d}", indices[index] + 1);

            elements[index] = (T)indices[index];
        }
    }

    void Mesh::setVertexMap(const std::vector<uint32_t>& map)
    {
        if (map.empty())
            throw love::Exception("Vertex map cannot be empty.");

        size_t maxValue        = this->getVertexCount();
        IndexDataType dataType = getIndexDataTypeFromMax(maxValue);
        DataFormat dataFormat  = getIndexDataFormat(dataType);

        bool recreate = this->indices.empty() || map.size() > this->indices.size();

        if (recreate)
            this->indices.resize(map.size());

        this->indexCount     = map.size();
        this->useIndexBuffer = true;
        this->indexDataType  = dataType;

        if (this->indexCount == 0)
            return;

        switch (dataType)
        {
            case INDEX_UINT16:
                copyToIndexBuffer<uint16_t>(map, this->indices.data(), maxValue);
                break;
            case INDEX_UINT32:
            default:
                copyToIndexBuffer<uint32_t>(map, this->indices.data(), maxValue);
                break;
        }

        this->indexDataModified = true;
    }

    void Mesh::setVertexMap(IndexDataType type, const void* data, size_t dataSize)
    {
        auto dataFormat = getIndexDataFormat(type);
        bool recreate   = this->indices.empty() || dataSize > (this->indices.size() * sizeof(uint16_t));

        if (recreate)
            this->indices.resize(dataSize / getIndexDataSize(type));

        this->indexCount     = dataSize / getIndexDataSize(type);
        this->useIndexBuffer = true;
        this->indexDataType  = type;

        if (this->indexCount == 0)
            return;

        std::memcpy(this->indices.data(), data, dataSize);
        this->indexDataModified = true;
    }

    void Mesh::setVertexMap()
    {
        this->useIndexBuffer = false;
    }

    template<typename T>
    static void copyFromIndexBuffer(const void* buffer, size_t count, std::vector<uint32_t>& indices)
    {
        const T* elements = (const T*)buffer;

        for (size_t i = 0; i < count; i++)
            indices.push_back((uint32_t)elements[i]);
    }

    bool Mesh::getVertexMap(std::vector<uint32_t>& map) const
    {
        if (!this->useIndexBuffer)
            return false;

        map.clear();

        if (this->indices.data() == nullptr || this->indexCount == 0)
            return false;

        map.reserve(this->indexCount);

        switch (this->indexDataType)
        {
            case INDEX_UINT16:
                copyFromIndexBuffer<uint16_t>(this->indices.data(), this->indexCount, map);
                break;
            case INDEX_UINT32:
            default:
                copyFromIndexBuffer<uint32_t>(this->indices.data(), this->indexCount, map);
                break;
        }

        return true;
    }

    void Mesh::setIndexBuffer(std::vector<uint16_t>& buffer)
    {
        if (this->buffer.data() == nullptr)
            throw love::Exception("Mesh must own its own vertex buffer.");

        this->indices        = buffer;
        this->useIndexBuffer = buffer.data() != nullptr;
        this->indexCount     = buffer.data() != nullptr ? buffer.size() : 0;

        if (buffer.data() != nullptr)
            this->indexDataType = INDEX_UINT16;
    }

    uint16_t* Mesh::getIndexBuffer()
    {
        return this->indices.data();
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
        this->primitiveType = mode;
    }

    PrimitiveType Mesh::getDrawMode() const
    {
        return this->primitiveType;
    }

    void Mesh::setDrawRange(int start, int count)
    {
        if (start < 0 || count <= 0)
            throw love::Exception("Invalid draw range: start = {:d}, count = {:d}", start, count);

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

        start = this->drawRange.getOffset();
        count = this->drawRange.getSize();

        return true;
    }

    void Mesh::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        this->drawInternal(graphics, matrix, 1, 0);
    }

    void Mesh::drawInternal(GraphicsBase* graphics, const Matrix4& matrix, int instanceCount, int argsIndex)
    {
        if (this->vertexCount <= 0)
            return;

        if (instanceCount > 1 && !graphics->getCapabilities().features[GraphicsBase::FEATURE_INSTANCING])
            throw love::Exception("Instanced drawing is not supported by the current graphics hardware.");

        // clang-format off
        if (this->primitiveType == PRIMITIVE_TRIANGLE_FAN && this->useIndexBuffer && this->indices.data() != nullptr)
            throw love::Exception("The 'fan' Mesh draw mode cannot be used with an index buffer / vertex map.");
        // clang-format on

        graphics->flushBatchedDraws();
        this->flush();

        if (ShaderBase::isDefaultActive())
            ShaderBase::attachDefault(ShaderBase::STANDARD_DEFAULT);

        GraphicsBase::TempTransform transform(graphics, matrix);

        int indexCount = this->indexCount;
        Range range    = this->drawRange;

        if (this->indices.data() != nullptr && this->indexCount > 0)
        {
            Range r(0, this->indexCount);

            if (range.isValid())
                r.intersect(range);

            DrawIndexedCommand command(nullptr, nullptr, nullptr);
            command.primitiveType     = this->primitiveType;
            command.indexType         = this->indexDataType;
            command.instanceCount     = instanceCount;
            command.texture           = this->texture;
            command.cullMode          = graphics->getMeshCullMode();
            command.indexBufferOffset = r.getOffset();
            command.indexCount        = (int)r.getSize();

            if (command.indexCount > 0)
                graphics->draw(command);
        }
        else
        {
            Range r(0, this->vertexCount);

            if (range.isValid())
                r.intersect(range);

            DrawCommand command {};
            command.primitiveType = this->primitiveType;
            command.vertexStart   = (int)r.getOffset();
            command.vertexCount   = (int)r.getSize();
            command.instanceCount = instanceCount;
            command.texture       = this->texture;
            command.cullMode      = graphics->getMeshCullMode();

            if (command.vertexCount > 0)
                graphics->draw(command);
        }
    }
} // namespace love
