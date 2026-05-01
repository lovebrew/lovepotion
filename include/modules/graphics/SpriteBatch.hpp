#pragma once

#include <cstring>
#include <unordered_map>

#include "common/Color.hpp"
#include "common/Matrix.hpp"
#include "common/Range.hpp"
#include "common/StrongRef.hpp"
#include "common/math.hpp"

#include "modules/graphics/Drawable.hpp"
#include "modules/graphics/Mesh.hpp"
#include "modules/graphics/vertex.hpp"

#include <vector>

namespace love
{
    class GraphicsBase;
    class TextureBase;
    class Quad;

    class SpriteBatch : public Drawable
    {
      public:
        static Type type;

        SpriteBatch(GraphicsBase* graphics, TextureBase* texture, int size, BufferDataUsage usage);

        virtual ~SpriteBatch();

        int add(const Matrix4& matrix, int index = -1);

        int add(Quad* quad, const Matrix4& matrix, int index = -1);

        int addLayer(int layer, const Matrix4& matrix, int index = -1);

        int addLayer(int layer, Quad* quad, const Matrix4& matrix, int index = -1);

        void clear();

        void flush();

        void setTexture(TextureBase* texture);

        TextureBase* getTexture() const;

        void setColor(const Color& color);

        Color getColor() const;

        int getCount() const;

        int getBufferSize() const;

        // void attachAttribute(const std::string&name, )

        void setDrawRange(int start, int count);

        void setDrawRange();

        bool getDrawRange(int& start, int& count) const;

        void draw(GraphicsBase* graphics, const Matrix4& matrix) override;

      private:
        void updateVertexAttributes(GraphicsBase* graphics);

        struct AttachedAttribute
        {
            StrongRef<BufferBase> buffer;
            StrongRef<Mesh> mesh;
            int index;
            int bindingIndex;
        };

        void setBufferSize(int newSize);

        StrongRef<TextureBase> texture;

        int size;
        int next;

        Color color;

        CommonFormat vertexFormat;
        size_t vertexStride;

        VertexAttributesID attributesID;
        BufferBindings bufferBindings;

        StrongRef<BufferBase> arrayBuffer;
        uint8_t* vertexData;

        Range modifiedSprites;

        std::unordered_map<std::string, AttachedAttribute> attachedAttributes;

        int rangeStart;
        int rangeCount;
    };
} // namespace love
