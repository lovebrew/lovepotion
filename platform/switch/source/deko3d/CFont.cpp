// #include "common/runtime.h"
// #include "deko3d/CFont.h"

// #include "deko3d/deko.h"

// #include "deko3d/graphics.h"
// #include "objects/font/font.h"

// #include "modules/font/fontmodule.h"
// #include "freetype/glyphdata.h"

// #include "common/module.h"
// #include "modules/graphics/graphics.h"

// #define FONT_TEXTURE_SIZE 0x200

// CFont::CFont() : textureCacheID(0),
//                  textureWidth(128),
//                  textureHeight(128),
//                  useSpacesAsTab(false)

// {}

// /*
// ** We first need to load the Face (font) via FreeType 2 memory loading.
// ** Once we do this, we create a big enough "empty" texture to copy the
// ** glyph data from whatever string we want into the texture itself to be
// ** rendered
// */
// void CFont::Load(love::Rasterizer * rasterizer, const love::Texture::Filter & filter)
// {
//     this->rasterizers = {rasterizer};

//     this->glyphs.clear();

//     while (true)
//     {
//         if ((rasterizer->GetHeight() * 0.8) * rasterizer->GetHeight() * 30 <= textureWidth * textureHeight)
//             break;

//         TextureSize nextsize = this->GetNextTextureSize();

//         if (nextsize.width <= textureWidth && nextsize.height <= textureHeight)
//             break;

//         textureWidth = nextsize.width;
//         textureHeight = nextsize.height;
//     }

//     LOG("Uhh crash?");
//     if (!rasterizer->HasGlyph(9)) // No tab character in the Rasterizer.
//         this->useSpacesAsTab = true;
//     LOG("Yay no crash?");
//     this->rowHeight = this->textureX = this->textureY = this->TEXTURE_PADDING;

//     this->CreateTexture();
// }

// CFont::~CFont()
// {
//     this->glyphs.clear();
//     m_scratch.destroy();
// }

// void CFont::CreateTexture()
// {
//     TextureSize size = {this->textureWidth, this->textureHeight};
//     TextureSize nextSize = this->GetNextTextureSize();
//     bool recreatetexture = false;

//     love::Image * texture = nullptr;
//     CImage image;

//     // If we have an existing texture already, we'll try replacing it with a
//     // larger-sized one rather than creating a second one. Having a single
//     // texture reduces texture switches and draw calls when rendering.
//     if ((nextSize.width > size.width || nextSize.height > size.height) && !images.empty())
//     {
//         recreatetexture = true;
//         size = nextSize;
//         images.pop_back();
//     }

//     LOG("Loading empty");
//     bool res = image.loadEmpty(*dk3d.GetImages(), *dk3d.GetData(), dk3d.GetDevice(), dk3d.GetTextureQueue(),
//                                size.width * size.height * 4, size.width, size.height, DkImageFormat_RGBA8_Unorm);

//     LOG("success? %d", res);

//     texture = new love::Image(love::Texture::TEXTURE_2D, size.width, size.height);
//     texture->SetTextureHandle(image);

//     if (recreatetexture)
//     {
//         textureCacheID++;

//         std::vector<uint32_t> glyphsToAdd;

//         for (const auto & glyphPair : glyphs)
//             glyphsToAdd.push_back(glyphPair.first);

//         glyphs.clear();

//         // for (uint32_t glyph : glyphsToAdd)
//         //     this->AddGlyph(glyph);
//     }

//     LOG("emplace it back bois");
//     images.emplace_back(texture, Acquire::NORETAIN);
//     LOG("done");
// }

// void CFont::FindGlyph(uint32_t glyph, Glyph & glyphData)
// {
//     const auto it = this->glyphs.find(glyph);

//     if (it != this->glyphs.end())
//         glyphData = it->second;

//     this->AddGlyph(glyph, glyphData);
// }

// void CFont::AddGlyph(uint32_t glyph, Glyph & glyphData)
// {
//     love::StrongReference<love::GlyphData> gd(this->GetRasterizerGlyphData(glyph), Acquire::NORETAIN);
//     LOG("glyphData is nullptr? %d", gd.Get() == nullptr);

//     int width  = gd->GetWidth();
//     int height = gd->GetHeight();

//     if (width + this->TEXTURE_PADDING * 2 < this->textureWidth && height + this->TEXTURE_PADDING * 2 < this->textureHeight)
//     {
//         if (this->textureX + width + this->TEXTURE_PADDING > this->textureWidth)
//         {
//             // Out of space - new row!
//             this->textureX = this->TEXTURE_PADDING;
//             this->textureY += this->rowHeight;
//             this->rowHeight = this->TEXTURE_PADDING;
//         }

//         if (this->textureY + height + this->TEXTURE_PADDING > this->textureHeight)
//         {
//             // Totally out of space - new texture!
//             this->CreateTexture();

//             // Makes sure the above code for checking if the glyph can fit at
//             // the current position in the texture is run again for this glyph.
//             return this->AddGlyph(glyph, glyphData);
//         }
//     }

//     Glyph g;

//     g.texture = nullptr;
//     g.spacing = floorf(gd->GetAdvance() / 1.0f + 0.5f);
//     memset(g.vertices, 0, sizeof(vertex::Vertex) * 4);

//     // Don't waste space on empty glyphs
//     if (width > 0 && height > 0)
//     {
//         g.texture = images.back();
//         // dk3d.LoadTextureBuffer(g.texture->GetTexture(), gd->GetData(), gd->GetSize(), g.texture, DkImageFormat_RGBA8_Unorm);
//     }

//     LOG("emplacing glyph %zu", glyph);
//     this->glyphs.emplace(glyph, g);
//     LOG("here's your glyph!");
//     glyphData = this->glyphs.at(glyph);
// }

// love::GlyphData * CFont::GetRasterizerGlyphData(uint32_t glyph)
// {
//     // Use spaces for the tab 'glyph'.
//     if (glyph == 9 && useSpacesAsTab)
//     {
//         love::GlyphData * spacegd = rasterizers[0]->GetGlyphData(32);

//         love::GlyphData::GlyphMetrics gm = {};

//         gm.advance  = spacegd->GetAdvance() * SPACES_PER_TAB;
//         gm.bearingX = spacegd->GetBearingX();
//         gm.bearingY = spacegd->GetBearingY();

//         spacegd->Release();

//         return new love::GlyphData(glyph, gm);
//     }

//     for (const love::StrongReference<love::Rasterizer> & r : rasterizers)
//     {
//         if (r->HasGlyph(glyph))
//             return r->GetGlyphData(glyph);
//     }

//     return rasterizers[0]->GetGlyphData(glyph);
// }



// CFont::