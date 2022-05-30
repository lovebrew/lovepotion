#include "objects/texture/texturec.h"
#include "common/bidirectionalmap.h"

#include "modules/graphics/graphics.h"

using namespace love::common;

love::Type Texture::type("Texture", &Drawable::type);

Texture::Texture(TextureType texType) :
    texType(texType),
    width(0),
    height(0),
    mipmapCount(1),
    samplerState {}
{}

Texture::~Texture()
{}

void Texture::InitQuad()
{
    Quad::Viewport v = { 0, 0, (double)this->width, (double)this->height };
    this->quad.Set(new love::Quad(v, this->width, this->height), Acquire::NORETAIN);
}

Texture::TextureType Texture::GetTextureType() const
{
    return this->texType;
}

int Texture::GetWidth(int mip) const
{
    return std::max(this->width >> mip, 1);
}

int Texture::GetHeight(int mip) const
{
    return std::max(this->height >> mip, 1);
}

const love::SamplerState& Texture::GetSamplerState() const
{
    return this->samplerState;
}

love::Quad* Texture::GetQuad() const
{
    return this->quad;
}

int Texture::GetMipmapCount() const
{
    return this->mipmapCount;
}

int Texture::GetTotalMipmapCount(int width, int height)
{
    return (int)log2(std::max(width, height)) + 1;
}

int Texture::GetTotalMipmapCount(int width, int height, int depth)
{
    return (int)log2(std::max(std::max(width, height), depth)) + 1;
}

// clang-format off
constexpr auto texTypes = BidirectionalMap<>::Create(
    "2d", Texture::TextureType::TEXTURE_2D
);
// clang-format on

bool Texture::GetConstant(const char* in, TextureType& out)
{
    return texTypes.Find(in, out);
}

bool Texture::GetConstant(TextureType in, const char*& out)
{
    return texTypes.ReverseFind(in, out);
}

std::vector<const char*> Texture::GetConstants(Texture::TextureType)
{
    return texTypes.GetNames();
}
