#pragma once

#include <common/drawable.hpp>
#include <common/range.hpp>
#include <common/strongreference.hpp>

#include <objects/texture/texture.tcc>

#include <utilities/driver/renderer/vertex.hpp>

#include <unordered_map>
#include <vector>

namespace love
{
    class Mesh : public Drawable
    {
      public:
        static Type type;

        Mesh(const void* data, size_t size, vertex::PrimitiveType mode);

        Mesh(int vertexCount, vertex::PrimitiveType mode);

        ~Mesh();

        void* CheckVertexDataOffset(size_t index, size_t* byteOffset);

        size_t GetVertexCount() const;

        size_t GetVertexStride() const;

        std::vector<vertex::Vertex> GetVertexBuffer() const;

        void* GetVertexData() const;

        void SetVertexDataModified(size_t offset, size_t dataSize);

        void Flush();

        void SetVertexMap(const std::vector<uint32_t>& map);

        void SetVertexMap(const void* data, size_t dataSize);

        void SetVertexMap();

        bool GetVertexMap(std::vector<uint32_t>& map) const;

        void SetIndexBuffer(std::vector<vertex::Vertex>& buffer);

        std::vector<vertex::Vertex> GetIndexBuffer() const;

        size_t GetIndexCount() const;

        void SetTexture(Texture<Console::Which>* texture);

        void SetTexture();

        Texture<Console::Which>* GetTexture() const;

        void SetDrawMode(vertex::PrimitiveType mode);

        vertex::PrimitiveType GetDrawMode() const;

        void SetDrawRange(int start, int count);

        void SetDrawRange();

        bool GetDrawRange(int& start, int& count) const;

        void Draw(Graphics<Console::Which>& graphics,
                  const Matrix4& matrix) override;

      private:
        void DrawInternal(Graphics<Console::Which>& graphics, const Matrix4& matrix,
                          int instanceCount, std::vector<vertex::Vertex>* indirectArgs,
                          int argsIndex);

        std::vector<vertex::Vertex> buffer;
        std::vector<vertex::Vertex> indexBuffer;

        size_t vertexCount;
        size_t vertexStride;

        size_t indexCount;
        vertex::PrimitiveType mode;
        StrongReference<Texture<Console::Which>> texture;

        Range drawRange;
        bool useIndexBuffer;
    };
} // namespace love
