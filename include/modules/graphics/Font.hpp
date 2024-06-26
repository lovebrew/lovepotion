#include "modules/graphics/Font.tcc"

namespace love
{
    class Font : public FontBase
    {
      public:
        Font(Rasterizer* rasterizer, const SamplerState& samplerState);

        virtual void createTexture() override;

        bool loadVolatile() override;

      private:
        virtual const FontBase::Glyph& addGlyph(TextShaper::GlyphIndex glyphIndex) override;
    };
} // namespace love
