#include "objects/texture/texturec.h"
#include "common/bidirectionalmap.h"

using namespace love::common;

love::Type Texture::type("Texture", &Drawable::type);

Texture::Filter Texture::defaultFilter;
Texture::FilterMode Texture::defaultMipmapFilter = Texture::FILTER_LINEAR;
float Texture::defaultMipmapSharpness            = 0.0f;

Texture::Texture(TextureType texType) :
    texType(texType),
    width(0),
    height(0),
    filter(defaultFilter),
    mipmapCount(1),
    wrap()
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

const Texture::Filter& Texture::GetFilter() const
{
    return this->filter;
}

const Texture::Wrap& Texture::GetWrap() const
{
    return this->wrap;
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

constexpr auto filterModes = BidirectionalMap<>::Create(
    "none",    Texture::FilterMode::FILTER_NONE,
    "linear",  Texture::FilterMode::FILTER_LINEAR,
    "nearest", Texture::FilterMode::FILTER_NEAREST
);

constexpr auto wrapModes = BidirectionalMap<>::Create(
    "clamp",          Texture::WrapMode::WRAP_CLAMP,
    "repeat",         Texture::WrapMode::WRAP_REPEAT,
    "clampzero",      Texture::WrapMode::WRAP_CLAMP_ZERO,
    "mirroredrepeat", Texture::WrapMode::WRAP_MIRRORED_REPEAT
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

bool Texture::GetConstant(const char* in, FilterMode& out)
{
    return filterModes.Find(in, out);
}

bool Texture::GetConstant(FilterMode in, const char*& out)
{
    return filterModes.ReverseFind(in, out);
}

std::vector<const char*> Texture::GetConstants(Texture::FilterMode)
{
    return filterModes.GetNames();
}

bool Texture::GetConstant(const char* in, WrapMode& out)
{
    return wrapModes.Find(in, out);
}

bool Texture::GetConstant(WrapMode in, const char*& out)
{
    return wrapModes.ReverseFind(in, out);
}

std::vector<const char*> Texture::GetConstants(Texture::WrapMode)
{
    return wrapModes.GetNames();
}
