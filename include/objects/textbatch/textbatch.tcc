#pragma once

#include <common/console.hpp>
#include <common/drawable.hpp>
#include <common/exception.hpp>
#include <common/strongreference.hpp>

#include <objects/font/font.tcc>
#include <objects/texture/texture.tcc>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class TextBatch : public Drawable
    {
      public:
        static inline Type type = Type("TextBatch", &Drawable::type);

        TextBatch(Font<Console::Which>* font) : font(font)
        {}

        Font<Console::Which>* GetFont() const
        {
            return this->font.Get();
        }

        void SetFont(Font<Console::Which>* font)
        {
            this->font.Set(font);
        }

        // virtual void Draw(Graphics<Console::Which>& graphics, Matrix4<Console::Which>& matrix) =
        // 0;

      protected:
        StrongReference<Font<Console::Which>> font;
    };
} // namespace love
