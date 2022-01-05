#include "objects/texture/texturec.h"

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

bool Texture::GetConstant(const char* in, TextureType& out)
{
    return texTypes.Find(in, out);
}

bool Texture::GetConstant(TextureType in, const char*& out)
{
    return texTypes.Find(in, out);
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
    return filterModes.Find(in, out);
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
    return wrapModes.Find(in, out);
}

std::vector<const char*> Texture::GetConstants(Texture::WrapMode)
{
    return wrapModes.GetNames();
}

// clang-format off
constexpr StringMap<Texture::TextureType, Texture::TEXTURE_MAX_ENUM>::Entry texTypeEntries[] =
{
    { "2d", Texture::TextureType::TEXTURE_2D }
};

constinit const StringMap<Texture::TextureType, Texture::TEXTURE_MAX_ENUM> Texture::texTypes(texTypeEntries);

constexpr StringMap<Texture::FilterMode, Texture::FILTER_MAX_ENUM>::Entry filterModeEntries[] =
{
    { "none",    Texture::FilterMode::FILTER_NONE    },
    { "linear",  Texture::FilterMode::FILTER_LINEAR  },
    { "nearest", Texture::FilterMode::FILTER_NEAREST }
};

constinit const StringMap<Texture::FilterMode, Texture::FILTER_MAX_ENUM> Texture::filterModes(filterModeEntries);

constexpr StringMap<Texture::WrapMode, Texture::WRAP_MAX_ENUM>::Entry wrapModeEntries[] =
{
    { "clamp",          Texture::WrapMode::WRAP_CLAMP           },
    { "repeat",         Texture::WrapMode::WRAP_REPEAT          },
    { "clampzero",      Texture::WrapMode::WRAP_CLAMP_ZERO      },
    { "mirroredrepeat", Texture::WrapMode::WRAP_MIRRORED_REPEAT }
};

constinit const StringMap<Texture::WrapMode, Texture::WRAP_MAX_ENUM> Texture::wrapModes(wrapModeEntries);
// clang-format on
