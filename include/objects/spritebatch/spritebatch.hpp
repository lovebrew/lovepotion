#pragma once

#include <common/drawable.hpp>

#include <common/color.hpp>
#include <common/matrix_ext.hpp>
#include <common/strongreference.hpp>

#include <objects/texture/texture.tcc>

#include <utilities/driver/renderer/vertex.hpp>

#include <vector>

namespace love
{
    class SpriteBatch : public Drawable
    {
      public:
        static Type type;

        SpriteBatch(Texture<Console::Which>* texture, int size);

        virtual ~SpriteBatch();

        int Add(const Matrix4<Console::Which>& matrix, int index = -1);

        int Add(Quad* quad, const Matrix4<Console::Which>& matrix, int index = -1);

        int AddLayer(int layer, const Matrix4<Console::Which>& matrix, int index = -1);

        int AddLayer(int layer, Quad* quad, const Matrix4<Console::Which>& matrix, int index = -1);

        void Clear();

        void Flush();

        void SetTexture(Texture<Console::Which>* texture);

        Texture<Console::Which>* GetTexture() const;

        void SetColor(const Color& color);

        void SetColor();

        Color GetColor() const;

        int GetCount() const;

        int GetBufferSize() const;

        void AttachAttribute(const std::string& name);

        void SetDrawRange(int start, int count);

        void SetDrawRange();

        bool GetDrawRange(int& start, int& count) const;

        void Draw(Graphics<Console::Which>& graphics,
                  const Matrix4<Console::Which>& matrix) override;

      private:
        void SetBufferSize(int size);

        StrongReference<Texture<Console::Which>> texture;

        int size;
        int next;

        Color color;

        std::vector<vertex::Vertex> buffer;

        vertex::CommonFormat format;
        size_t vertexStride;

        int rangeStart;
        int rangeCount;
    };
} // namespace love