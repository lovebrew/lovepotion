#pragma once

#include "common/Range.hpp"
#include "common/StrongRef.hpp"
#include "common/int.hpp"

#include "modules/graphics/Buffer.tcc"
#include "modules/graphics/Drawable.hpp"
#include "modules/graphics/Texture.tcc"

#include <vector>

namespace love
{
    class GraphicsBase;

    class Mesh : public Drawable
    {
      public:
        static Type type;

        Mesh(GraphicsBase* graphics, const std::vector<BufferBase::DataDeclaration>& format, const void* data,
             size_t size, PrimitiveType mode, BufferDataUsage usage);

        Mesh(GraphicsBase* graphics, const std::vector<BufferBase::DataDeclaration>& format, int vertexCount,
             PrimitiveType mode, BufferDataUsage usage);

        virtual ~Mesh();

        void* checkVertexDataOffset(size_t index, size_t* byteOffset);

        const std::vector<BufferBase::DataMember>& getVertexFormat() const;

        size_t getVertexCount() const;

        size_t getVertexStride() const;

        Vertex* getVertexBuffer();

        void* getVertexData();

        void setVertexDataModified(size_t offset, size_t size);

        void flush();

        void setVertexMap(const std::vector<uint32_t>& map);

        void setVertexMap(IndexDataType type, const void* data, size_t size);

        void setVertexMap();

        bool getVertexMap(std::vector<uint32_t>& map) const;

        void setIndexBuffer(std::vector<uint16_t>& buffer);

        uint16_t* getIndexBuffer();

        size_t getIndexCount() const;

        void setTexture(TextureBase* texture);

        void setTexture();

        TextureBase* getTexture() const;

        void setDrawMode(PrimitiveType mode);

        PrimitiveType getDrawMode() const;

        void setDrawRange(int start, int count);

        void setDrawRange();

        bool getDrawRange(int& start, int& count) const;

        void draw(GraphicsBase* graphics, const Matrix4& matrix);

        void draw(GraphicsBase* graphics, const Matrix4& matrix, int instanceCount);

        static std::vector<BufferBase::DataDeclaration> getDefaultVertexFormat();

      private:
        struct IndexBuffer : public Resource
        {
          public:
            IndexBuffer(const std::vector<uint16_t>& data) : data(data)
            {}

            ptrdiff_t getHandle() const override
            {
                return (ptrdiff_t)this->data.data();
            }

          private:
            std::vector<uint16_t> data;
        };

        friend class SpriteBatch;

        void drawInternal(GraphicsBase* graphics, const Matrix4& matrix, int instanceCount, int argsIndex);
        std::vector<BufferBase::DataMember> vertexFormat;

        std::vector<Vertex> buffer;
        Vertex* vertexBuffer = nullptr;

        std::vector<uint16_t> indices;
        uint16_t* indexBuffer = nullptr;

        Range modifiedVertexData = Range();

        size_t vertexCount  = 0;
        size_t vertexStride = 0;

        bool indexDataModified      = false;
        bool useIndexBuffer         = false;
        size_t indexCount           = 0;
        IndexDataType indexDataType = INDEX_UINT16;

        PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;

        Range drawRange = Range();
        StrongRef<TextureBase> texture;
    };
} // namespace love
