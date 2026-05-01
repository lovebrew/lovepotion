#pragma once

#include "common/Map.hpp"
#include "common/Range.hpp"
#include "common/int.hpp"
#include "common/math.hpp"

#include "modules/graphics/Buffer.tcc"
#include "modules/graphics/Drawable.hpp"
#include "modules/graphics/Texture.tcc"
#include "modules/graphics/vertex.hpp"

#include <unordered_map>
#include <vector>

namespace love
{
    class GraphicsBase;

    class Mesh final : public Drawable
    {
      public:
        struct BufferAttribute
        {
            std::string name;
            StrongRef<BufferBase> buffer;
            StrongRef<Mesh> mesh;
            std::string nameInBuffer;
            int bindingLocationInBuffer = -1;
            int indexInBuffer           = 0;
            int startArrayIndex         = 0;
            AttributeStep step          = STEP_PER_VERTEX;
            int bindingLocation         = -1;
            bool enabled                = false;
        };

        static Type type;

        Mesh(GraphicsBase* graphics, const BufferBase::BufferFormat& vertexFormat, const void* data,
             size_t dataSize, PrimitiveType mode, BufferDataUsage usage);

        Mesh(GraphicsBase* graphics, const BufferBase::BufferFormat& vertexFormat, int vertexCount,
             PrimitiveType mode, BufferDataUsage usage);

        Mesh(const std::vector<BufferAttribute>& attributes, PrimitiveType mode);

        virtual ~Mesh();

        void* checkVertexDataOffset(size_t offset, size_t* byteOffset);

        size_t getVertexCount() const;

        size_t getVertexStride() const;

        BufferBase* getVertexBuffer() const;

        const std::vector<BufferBase::DataMember>& getVertexFormat() const;

        void setAttributeEnabled(const std::string& name, bool enabled);

        bool isAttributeEnabled(const std::string& name) const;

        void setAttributeEnabled(int location, bool enabled);

        bool isAttributeEnabled(int location) const;

        void attachAttribute(const std::string& name, BufferBase* buffer, Mesh* mesh,
                             const std::string& attachName, int startIndex = 0,
                             AttributeStep step = STEP_PER_VERTEX);

        bool detachAttribute(const std::string& name);

        void attachAttribute(int location, BufferBase* buffer, Mesh* mesh, int attachBindingLocation,
                             int startIndex = 0, AttributeStep step = STEP_PER_VERTEX);

        bool detachAttribute(int location);

        const std::vector<BufferAttribute>& getAttachedAttributes() const;

        void* getVertexData() const;

        void setVertexDataModified(size_t offset, size_t size);

        void flush();

        void setVertexMap(const std::vector<uint32_t>& map);

        void setVertexMap(IndexDataType type, const void* data, size_t size);

        void setVertexMap();

        bool getVertexMap(std::vector<uint32_t>& map) const;

        void setIndexBuffer(BufferBase* indexBuffer);

        BufferBase* getIndexBuffer() const;

        size_t getIndexCount() const;

        void setTexture(TextureBase* texture);

        void setTexture();

        TextureBase* getTexture() const;

        void setDrawMode(PrimitiveType mode);

        PrimitiveType getDrawMode() const;

        void setDrawRange(int start, int count);

        void setDrawRange();

        bool getDrawRange(int& start, int& count) const;

        void draw(GraphicsBase* graphics, const Matrix4& transform) override;

        void drawInstanced(GraphicsBase* graphics, const Matrix4& transform, int instanceCount);

        void drawIndirect(GraphicsBase* graphics, const Matrix4& transform, BufferBase* indirectArgs,
                          int argsIndex);

        static std::vector<BufferBase::DataDeclaration> getDefaultVertexFormat();

      private:
        friend class SpriteBatch;

        void setupAttachedAttributes();

        int getAttachedAttributeIndex(const std::string& name) const;

        int getAttachedAttributeIndex(int location) const;

        void finalizeAttribute(BufferAttribute& attribute) const;

        void updateVertexAttributes(GraphicsBase* graphics);

        void drawInternal(GraphicsBase* graphics, const Matrix4& transform, int instanceCount,
                          BufferBase* indirectArgs, int argsIndex);

        std::vector<BufferBase::DataMember> vertexFormat;
        std::vector<BufferAttribute> attachedAttributes;
        VertexAttributesID attributesID;

        StrongRef<BufferBase> vertexBuffer;
        uint8_t* vertexData      = nullptr;
        Range modifiedVertexData = Range();

        size_t vertexCount  = 0;
        size_t vertexStride = 0;

        StrongRef<BufferBase> indexBuffer;
        uint8_t* indexData      = nullptr;
        bool indexDataModified  = false;
        bool useIndexBuffer     = false;
        size_t indexCount       = 0;
        IndexDataType indexType = INDEX_UINT16;

        PrimitiveType drawMode = PRIMITIVE_TRIANGLES;
        Range drawRange        = Range();

        StrongRef<TextureBase> texture;
        BufferBindings bufferBindings;
    };
} // namespace love
