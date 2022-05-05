#pragma once

#include "common/colors.h"
#include "common/matrix.h"

#include "objects/drawable/drawable.h"

namespace love
{
    class Graphics;
    class Texture;
    class Quad;

    namespace common
    {
        class SpriteBatch
        {
          public:
            static love::Type type;

            SpriteBatch(Graphics* graphics, love::Texture* texture, int size);

            virtual ~SpriteBatch();

            int Add(const Matrix4& matrix, int index = -1);

            virtual int Add(love::Quad* quad, const Matrix4& matrix, int index = -1) = 0;

            void Clear();

            virtual void Flush() = 0;

            void SetTexture(love::Texture* texture);

            love::Texture* GetTexture() const;

            void SetColor(const Colorf& color);

            void SetColor();

            Colorf GetColor(bool& active) const;

            int GetCount() const;

            int GetBufferSize() const;

            void SetDrawRange(int start, int count);

            void SetDrawRange();

            bool GetDrawRange(int& start, int& count) const;

            virtual void Draw(Graphics* graphics, const Matrix4& matrix) = 0;

          protected:
            virtual void SetBufferSize(int newSize) = 0;

            StrongReference<love::Texture> texture;

            int size;
            int next;

            Colorf color;
            bool colorActive;

            int rangeStart;
            int rangeCount;
        };
    } // namespace common
} // namespace love
