#include <objects/texture/wrap_texture.hpp>

#include <objects/imagedata/wrap_imagedata.hpp>

using namespace love;
using Texture = love::Texture<Console::Which>;

::Texture* Wrap_Texture::CheckTexture(lua_State* L, int index)
{
    return luax::CheckType<::Texture>(L, 1);
}

static int luax_optMipmap(lua_State* L, ::Texture* texture, int index)
{
    int mipmap = 0;
    if (!lua_isnoneornil(L, index))
    {
        mipmap = luaL_checkinteger(L, index) - 1;

        if (mipmap < 0 || mipmap >= texture->GetMipmapCount())
            luaL_error(L, "Invalid mipmap index: %d", mipmap + 1);
    }

    return mipmap;
}

int Wrap_Texture::GetWidth(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetWidth(luax_optMipmap(L, self, 2)));

    return 1;
}

int Wrap_Texture::GetHeight(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetHeight(luax_optMipmap(L, self, 2)));

    return 1;
}

int Wrap_Texture::GetDimensions(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);
    int mipmap = luax_optMipmap(L, self, 2);

    lua_pushnumber(L, self->GetWidth(mipmap));
    lua_pushnumber(L, self->GetHeight(mipmap));

    return 2;
}

int Wrap_Texture::GetDepth(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetDepth(luax_optMipmap(L, self, 2)));

    return 1;
}

int Wrap_Texture::GetLayerCount(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetLayerCount());

    return 1;
}

int Wrap_Texture::GetMipmapCount(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetMipmapCount());

    return 1;
}

int Wrap_Texture::GetPixelWidth(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetPixelWidth(luax_optMipmap(L, self, 2)));

    return 1;
}

int Wrap_Texture::GetPixelHeight(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetPixelHeight(luax_optMipmap(L, self, 2)));

    return 1;
}

int Wrap_Texture::GetPixelDimensions(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);
    int mipmap = luax_optMipmap(L, self, 2);

    lua_pushnumber(L, self->GetPixelWidth(mipmap));
    lua_pushnumber(L, self->GetPixelHeight(mipmap));

    return 2;
}

int Wrap_Texture::GetFormat(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    std::optional<const char*> type;
    if (!(type = pixelFormats.ReverseFind(self->GetPixelFormat())))
        luaL_error(L, "Unknown pixelformat.");

    lua_pushstring(L, *type);

    return 1;
}

int Wrap_Texture::GetTextureType(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    std::optional<const char*> type;
    if (!(type = ::Texture::textureTypes.ReverseFind(self->GetTextureType())))
        return luaL_error(L, "Unknown texture type.");

    lua_pushstring(L, *type);

    return 1;
}

int Wrap_Texture::GetDPIScale(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushnumber(L, self->GetDPIScale());

    return 1;
}

int Wrap_Texture::IsFormatLinear(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushboolean(L, self->IsFormatLinear());

    return 1;
}

int Wrap_Texture::IsCompressed(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    luax::PushBoolean(L, self->IsCompressed());

    return 1;
}

int Wrap_Texture::GetMSAA(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    lua_pushinteger(L, self->GetMSAA());

    return 1;
}

int Wrap_Texture::SetFilter(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    SamplerState state = self->GetSamplerState();

    const char* minName = luaL_checkstring(L, 2);
    const char* magName = luaL_optstring(L, 3, minName);

    std::optional<SamplerState::FilterMode> min;
    std::optional<SamplerState::FilterMode> mag;

    if (!(min = SamplerState::filterModes.Find(minName)))
        return luax::EnumError(L, "filter mode", SamplerState::filterModes, minName);

    if (!(mag = SamplerState::filterModes.Find(magName)))
        return luax::EnumError(L, "filter mode", SamplerState::filterModes, magName);

    int maxAnisotropy   = luaL_optnumber(L, 4, 1.0);
    state.maxAnisotropy = std::clamp<uint8_t>(maxAnisotropy, 1, LOVE_INT8_MAX);

    luax::CatchException(L, [&]() { self->SetSamplerState(state); });

    return 0;
}

int Wrap_Texture::GetFilter(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    const SamplerState& state = self->GetSamplerState();

    std::optional<const char*> minName;
    std::optional<const char*> magName;

    if (!(minName = SamplerState::filterModes.ReverseFind(state.minFilter)))
        luaL_error(L, "Unknown filter mode.");

    if (!(magName = SamplerState::filterModes.ReverseFind(state.magFilter)))
        luaL_error(L, "Unknown filter mode.");

    lua_pushstring(L, *minName);
    lua_pushstring(L, *magName);
    lua_pushnumber(L, state.maxAnisotropy);

    return 3;
}

int Wrap_Texture::SetMipmapFilter(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    SamplerState state = self->GetSamplerState();

    if (lua_isnoneornil(L, 2))
        state.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;
    else
    {
        const char* mipmapMode = luaL_checkstring(L, 2);

        std::optional<SamplerState::MipmapFilterMode> mode;
        if (!(mode = SamplerState::mipMapFilterModes.Find(mipmapMode)))
            return luax::EnumError(L, "filter mode", SamplerState::mipMapFilterModes, mipmapMode);

        state.mipmapFilter = *mode;
    }

    state.lodBias = -((float)luaL_optnumber(L, 3, 0.0));

    luax::CatchException(L, [&]() { self->SetSamplerState(state); });

    return 0;
}

int Wrap_Texture::GetMipmapFilter(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    const SamplerState& state = self->GetSamplerState();
    std::optional<const char*> mipmapMode;

    if (mipmapMode = SamplerState::mipMapFilterModes.ReverseFind(state.mipmapFilter))
        lua_pushstring(L, *mipmapMode);
    else
        lua_pushnil(L);

    lua_pushnumber(L, -state.lodBias);

    return 2;
}

int Wrap_Texture::SetWrap(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    SamplerState state = self->GetSamplerState();

    const char* wrapUName = luaL_checkstring(L, 2);
    const char* wrapVName = luaL_checkstring(L, 3);
    const char* wrapWName = luaL_checkstring(L, 4);

    std::optional<SamplerState::WrapMode> wrapU;
    if (!(wrapU = SamplerState::wrapModes.Find(wrapUName)))
        return luax::EnumError(L, "wrap mode", SamplerState::wrapModes, wrapUName);

    state.wrapU = *wrapU;

    std::optional<SamplerState::WrapMode> wrapV;
    if (!(wrapV = SamplerState::wrapModes.Find(wrapVName)))
        return luax::EnumError(L, "wrap mode", SamplerState::wrapModes, wrapVName);

    state.wrapV = *wrapV;

    std::optional<SamplerState::WrapMode> wrapW;
    if (!(wrapW = SamplerState::wrapModes.Find(wrapWName)))
        return luax::EnumError(L, "wrap mode", SamplerState::wrapModes, wrapWName);

    state.wrapW = *wrapW;

    luax::CatchException(L, [&]() { self->SetSamplerState(state); });

    return 0;
}

int Wrap_Texture::GetWrap(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    const SamplerState& state = self->GetSamplerState();

    std::optional<const char*> wrapUName;
    std::optional<const char*> wrapVName;
    std::optional<const char*> wrapWName;

    if (!(wrapUName = SamplerState::wrapModes.ReverseFind(state.wrapU)))
        return luaL_error(L, "Unknown wrap mode.");

    if (!(wrapVName = SamplerState::wrapModes.ReverseFind(state.wrapV)))
        return luaL_error(L, "Unkown wrap mode.");

    if (!(wrapWName = SamplerState::wrapModes.ReverseFind(state.wrapW)))
        return luaL_error(L, "Unknown wrap mode.");

    lua_pushstring(L, *wrapUName);
    lua_pushstring(L, *wrapVName);
    lua_pushstring(L, *wrapWName);

    return 3;
}

int Wrap_Texture::IsRenderTarget(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    luax::PushBoolean(L, self->IsRenderTarget());

    return 1;
}

int Wrap_Texture::IsComputeWritable(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    luax::PushBoolean(L, self->IsComputeWritable());

    return 1;
}

int Wrap_Texture::IsReadable(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    luax::PushBoolean(L, self->IsReadable());

    return 1;
}

int Wrap_Texture::SetDepthSampleMode(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    SamplerState state = self->GetSamplerState();
    state.depthSampleMode.reset();

    if (!lua_isnoneornil(L, 2))
    {
        const char* string = luaL_checkstring(L, 2);
        std::optional<RenderState::CompareMode> compareMode;

        if (!(compareMode = RenderState::compareModes.Find(string)))
            return luax::EnumError(L, "compare mode", RenderState::compareModes, string);
    }

    luax::CatchException(L, [&]() { self->SetSamplerState(state); });

    return 0;
}

int Wrap_Texture::GetDepthSampleMode(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    const SamplerState& state = self->GetSamplerState();

    if (state.depthSampleMode.has_value())
    {
        std::optional<const char*> string;
        if (!(string = RenderState::compareModes.ReverseFind(state.depthSampleMode.value())))
            return luaL_error(L, "Unknown compare mode.");

        lua_pushstring(L, *string);
    }
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Texture::GetMipmapMode(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    std::optional<const char*> mode;
    if (!(mode = ::Texture::mipmapModes.ReverseFind(self->GetMipmapsMode())))
        return luax::EnumError(L, "mipmap mode", ::Texture::mipmapModes, *mode);

    lua_pushstring(L, *mode);

    return 1;
}

int Wrap_Texture::GenerateMipmaps(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);

    luax::CatchException(L, [&]() { self->GenerateMipmaps(); });

    return 0;
}

int Wrap_Texture::ReplacePixels(lua_State* L)
{
    auto* self = Wrap_Texture::CheckTexture(L, 1);
    auto* data = Wrap_ImageData::CheckImageData(L, 2);

    int slice  = 0;
    int mipmap = 0;

    int x = 0;
    int y = 0;

    bool autoMipmaps = self->GetMipmapsMode() == ::Texture::MIPMAPS_AUTO;
    bool reloadMipmaps;

    if (self->GetTextureType() != ::Texture::TEXTURE_2D)
        slice = luaL_checkinteger(L, 3) - 1;

    mipmap = luaL_optinteger(L, 4, 1) - 1;

    if (!lua_isnoneornil(L, 5))
    {
        x = luaL_checkinteger(L, 5);
        y = luaL_checkinteger(L, 6);

        if (autoMipmaps)
            reloadMipmaps = luax::OptBoolean(L, 7, autoMipmaps);
    }

    // clang-format off
    luax::CatchException(L, [&]() {
        self->ReplacePixels(data, slice, mipmap, x, y, reloadMipmaps);
    });
    // clang-format on

    return 0;
}

/* todo */
int Wrap_Texture::RenderTo(lua_State* L)
{
    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getTextureType",     Wrap_Texture::GetTextureType     },
    { "getWidth",           Wrap_Texture::GetWidth           },
    { "getHeight",          Wrap_Texture::GetHeight          },
    { "getDimensions",      Wrap_Texture::GetDimensions      },
    { "getDepth",           Wrap_Texture::GetDepth           },
    { "getLayerCount",      Wrap_Texture::GetLayerCount      },
    { "getMipmapCount",     Wrap_Texture::GetMipmapCount     },
    { "getPixelWidth",      Wrap_Texture::GetPixelWidth      },
    { "getPixelHeight",     Wrap_Texture::GetPixelHeight     },
    { "getPixelDimensions", Wrap_Texture::GetPixelDimensions },
    { "getDPIScale",        Wrap_Texture::GetDPIScale        },
    { "isFormatLinear",     Wrap_Texture::IsFormatLinear     },
    { "isCompressed",       Wrap_Texture::IsCompressed       },
    { "getMSAA",            Wrap_Texture::GetMSAA            },
    { "setFilter",          Wrap_Texture::SetFilter          },
    { "getFilter",          Wrap_Texture::GetFilter          },
    { "setMipmapFilter",    Wrap_Texture::SetMipmapFilter    },
    { "getMipmapFilter",    Wrap_Texture::GetMipmapFilter    },
    { "setWrap",            Wrap_Texture::SetWrap            },
    { "getWrap",            Wrap_Texture::GetWrap            },
    { "getFormat",          Wrap_Texture::GetFormat          },
    { "isRenderTarget",     Wrap_Texture::IsRenderTarget     },
    { "isComputeWritable",  Wrap_Texture::IsComputeWritable  },
    { "isReadable",         Wrap_Texture::IsReadable         },
    { "getMipmapMode",      Wrap_Texture::GetMipmapMode      },
    { "getDepthSampleMode", Wrap_Texture::GetDepthSampleMode },
    { "setDepthSampleMode", Wrap_Texture::SetDepthSampleMode },
    { "generateMipmaps",    Wrap_Texture::GenerateMipmaps    },
    { "replacePixels",      Wrap_Texture::ReplacePixels      },
    { "renderTo",           Wrap_Texture::RenderTo           }
};
// clang-format on

int Wrap_Texture::Register(lua_State* L)
{
    return luax::RegisterType(L, &::Texture::type, functions);
}
