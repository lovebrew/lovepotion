// #pragma once

// #include <objects/truetyperasterizer/truetyperasterizer.tcc>

// #include <3ds.h>

// namespace love
// {
//     template<>
//     class TrueTypeRasterizer<Console::CTR> : public TrueTypeRasterizer<Console::ALL>
//     {
//       public:
//         TrueTypeRasterizer(FT_Library library, Data* data, int size, float dpiSacale,
//                            Hinting hinting);

//         ~TrueTypeRasterizer();

//         int GetLineHeight() const override;

//         int GetGlyphSpacing(uint32_t glyph) const override;

//         int GetGlyphIndex(uint32_t glyph) const override;

//         GlyphData* GetGlyphDataForIndex(int index) const override;

//         int GetGlyphCount() const override;

//         bool HasGlyph(uint32_t glyph) const override;

//         float GetKerning(uint32_t left, uint32_t right) const override;

//         DataType GetDataType() const override;

//         TextShaper* NewTextShaper() override;

//         ptrdiff_t GetHandle() const override
//         {
//             return (ptrdiff_t)&face;
//         }

//         static bool Accepts(FT_Library library, Data* data);

//         float Scale(uint8_t value) const
//         {
//             return this->scale * value;
//         }

//       private:
//         float scale;
//         mutable int glyphCount;
//     };
// } // namespace love
