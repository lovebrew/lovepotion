#pragma once

#include "objects/quad/quad.h"
#include "objects/spritebatch/spritebatchc.h"

namespace love
{
    class SpriteBatch : public common::SpriteBatch
    {
      public:
        SpriteBatch(Graphics* graphics, love::Texture* texture, int size);

        virtual ~SpriteBatch();

        using common::SpriteBatch::Add;

        virtual int Add(Quad* quad, const Matrix4& matrix, int index = -1) override;

        virtual void Flush() override;

        virtual void Draw(Graphics* graphics, const Matrix4& matrix) override;

      protected:
        virtual void SetBufferSize(int newSize) override;
    };
} // namespace love
