#include "modules/graphics/wrap_Texture.hpp"
#include "modules/graphics/Graphics.hpp"

using namespace love;

int Wrap_Texture::getTextureType(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    std::string_view type;
    if (!Texture::getConstant(self->getTextureType(), type))
        return luax_enumerror(L, "texture type", Texture::TextureTypes, type);

    luax_pushstring(L, type);
    return 1;
}

static int optMipmap(lua_State* L, TextureBase* texture, int index)
{
    int mipmap = 0;

    if (!lua_isnoneornil(L, index))
    {
        mipmap = luaL_checkinteger(L, index) - 1;

        if (mipmap < 0 || mipmap >= texture->getMipmapCount())
            luaL_error(L, "Invalid mipmap index: %d", mipmap + 1);
    }

    return mipmap;
}

int Wrap_Texture::getWidth(lua_State* L)
{
    auto* self       = luax_checktexture(L, 1);
    const int mipmap = optMipmap(L, self, 2);

    lua_pushnumber(L, self->getWidth(mipmap));

    return 1;
}

int Wrap_Texture::getHeight(lua_State* L)
{
    auto* self       = luax_checktexture(L, 1);
    const int mipmap = optMipmap(L, self, 2);

    lua_pushnumber(L, self->getHeight(mipmap));

    return 1;
}

int Wrap_Texture::getDimensions(lua_State* L)
{
    auto* self       = luax_checktexture(L, 1);
    const int mipmap = optMipmap(L, self, 2);

    lua_pushnumber(L, self->getWidth(mipmap));
    lua_pushnumber(L, self->getHeight(mipmap));

    return 2;
}

int Wrap_Texture::getDepth(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushnumber(L, self->getDepth());

    return 1;
}

int Wrap_Texture::getLayerCount(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushnumber(L, self->getLayerCount());

    return 1;
}

int Wrap_Texture::getMipmapCount(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushnumber(L, self->getMipmapCount());

    return 1;
}

int Wrap_Texture::getPixelWidth(lua_State* L)
{
    auto* self       = luax_checktexture(L, 1);
    const int mipmap = optMipmap(L, self, 2);

    lua_pushnumber(L, self->getPixelWidth(mipmap));

    return 1;
}

int Wrap_Texture::getPixelHeight(lua_State* L)
{
    auto* self       = luax_checktexture(L, 1);
    const int mipmap = optMipmap(L, self, 2);

    lua_pushnumber(L, self->getPixelHeight(mipmap));

    return 1;
}

int Wrap_Texture::getPixelDimensions(lua_State* L)
{
    auto* self       = luax_checktexture(L, 1);
    const int mipmap = optMipmap(L, self, 2);

    lua_pushnumber(L, self->getPixelWidth(mipmap));
    lua_pushnumber(L, self->getPixelHeight(mipmap));

    return 2;
}

int Wrap_Texture::getDPIScale(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushnumber(L, self->getDPIScale());

    return 1;
}

int Wrap_Texture::isFormatLinear(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushboolean(L, self->isFormatLinear());

    return 1;
}

int Wrap_Texture::isCompressed(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushboolean(L, self->isCompressed());

    return 1;
}

int Wrap_Texture::getMSAA(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushnumber(L, self->getMSAA());

    return 1;
}

int Wrap_Texture::setFilter(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    SamplerState state = self->getSamplerState();

    const char* minFilter = luaL_checkstring(L, 2);
    const char* magFilter = luaL_checkstring(L, 3);

    if (!SamplerState::getConstant(minFilter, state.minFilter))
        return luax_enumerror(L, "minification filter", SamplerState::FilterModes, minFilter);

    if (!SamplerState::getConstant(magFilter, state.magFilter))
        return luax_enumerror(L, "magnification filter", SamplerState::FilterModes, magFilter);

    state.maxAnisotropy = std::clamp((int)luaL_optnumber(L, 4, 1.0), 1, LOVE_UINT8_MAX);

    luax_catchexcept(L, [&]() { self->setSamplerState(state); });

    return 0;
}

int Wrap_Texture::getFilter(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    const auto& state = self->getSamplerState();

    std::string_view minFilter;
    std::string_view magFilter;

    if (!SamplerState::getConstant(state.minFilter, minFilter))
        return luax_enumerror(L, "minification filter", SamplerState::FilterModes, minFilter);

    if (!SamplerState::getConstant(state.magFilter, magFilter))
        return luax_enumerror(L, "magnification filter", SamplerState::FilterModes, magFilter);

    luax_pushstring(L, minFilter);
    luax_pushstring(L, magFilter);
    lua_pushnumber(L, state.maxAnisotropy);

    return 3;
}

int Wrap_Texture::setMipmapFilter(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    auto state = self->getSamplerState();

    if (lua_isnoneornil(L, 2))
        state.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;
    else
    {
        const char* mipmapType = luaL_checkstring(L, 2);
        if (!SamplerState::getConstant(mipmapType, state.mipmapFilter))
            return luax_enumerror(L, "mipmap filter", SamplerState::MipmapFilterModes, mipmapType);
    }

    state.lodBias = -((float)luaL_optnumber(L, 3, 0.0));

    luax_catchexcept(L, [&]() { self->setSamplerState(state); });

    return 0;
}

int Wrap_Texture::getMipmapFilter(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    const auto& state = self->getSamplerState();

    std::string_view mipmapFilter;

    if (SamplerState::getConstant(state.mipmapFilter, mipmapFilter))
        luax_pushstring(L, mipmapFilter);
    else
        lua_pushnil(L);

    lua_pushnumber(L, -state.lodBias);

    return 2;
}

int Wrap_Texture::setWrap(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    auto state = self->getSamplerState();

    const char* wrapS = luaL_checkstring(L, 2);
    const char* wrapT = luaL_optstring(L, 3, wrapS);
    const char* wrapR = luaL_optstring(L, 4, wrapS);

    if (!SamplerState::getConstant(wrapS, state.wrapU))
        return luax_enumerror(L, "wrap mode", SamplerState::WrapModes, wrapS);

    if (!SamplerState::getConstant(wrapT, state.wrapV))
        return luax_enumerror(L, "wrap mode", SamplerState::WrapModes, wrapT);

    if (!SamplerState::getConstant(wrapR, state.wrapW))
        return luax_enumerror(L, "wrap mode", SamplerState::WrapModes, wrapR);

    luax_catchexcept(L, [&]() { self->setSamplerState(state); });

    return 0;
}

int Wrap_Texture::getWrap(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    const auto& state = self->getSamplerState();

    std::string_view wrapS;
    std::string_view wrapT;
    std::string_view wrapR;

    if (!SamplerState::getConstant(state.wrapU, wrapS))
        return luaL_error(L, "Unknown wrap mode.");

    if (!SamplerState::getConstant(state.wrapV, wrapT))
        return luaL_error(L, "Unknown wrap mode.");

    if (!SamplerState::getConstant(state.wrapW, wrapR))
        return luaL_error(L, "Unknown wrap mode.");

    luax_pushstring(L, wrapS);
    luax_pushstring(L, wrapT);
    luax_pushstring(L, wrapR);

    return 3;
}

int Wrap_Texture::getFormat(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    auto pixelFormat = self->getPixelFormat();
    std::string_view format;

    if (!love::getConstant(pixelFormat, format))
        return luaL_error(L, "Unknown pixel format.");

    luax_pushstring(L, format);
    return 1;
}

int Wrap_Texture::isCanvas(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushboolean(L, self->isRenderTarget());

    return 1;
}

int Wrap_Texture::isComputeWritable(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushboolean(L, self->isComputeWritable());

    return 1;
}

int Wrap_Texture::isReadable(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    lua_pushboolean(L, self->isReadable());

    return 1;
}

int Wrap_Texture::getViewFormats(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    const auto& formats = self->getViewFormats();

    lua_createtable(L, (int)formats.size(), 0);

    for (int index = 0; index < (int)formats.size(); index++)
    {
        std::string_view name;
        if (!love::getConstant(formats[index], name))
            return luaL_error(L, "Unknown pixel format.");

        luax_pushstring(L, name);
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_Texture::setDepthSampleMode(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    auto state = self->getSamplerState();

    state.depthSampleMode.hasValue = false;
    if (!lua_isnoneornil(L, 2))
    {
        const char* name = luaL_checkstring(L, 2);

        state.depthSampleMode.hasValue = true;
        if (!love::getConstant(name, state.depthSampleMode.value))
            return luax_enumerror(L, "depth sample mode", love::CompareModes, name);
    }

    luax_catchexcept(L, [&]() { self->setSamplerState(state); });

    return 0;
}

int Wrap_Texture::getDepthSampleMode(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    const auto& state = self->getSamplerState();

    if (state.depthSampleMode.hasValue)
    {
        std::string_view name;
        if (!love::getConstant(state.depthSampleMode.value, name))
            return luaL_error(L, "Unknown compare mode.");

        luax_pushstring(L, name);
    }
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Texture::getMipmapMode(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    std::string_view name;
    if (!Texture::getConstant(self->getMipmapsMode(), name))
        return luax_enumerror(L, "mipmap mode", Texture::MipmapsModes, name);

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Texture::generateMipmaps(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    luax_catchexcept(L, [&]() { self->generateMipmaps(); });

    return 0;
}

// TODO: Implement this
int Wrap_Texture::replacePixels(lua_State* L)
{
    return 0;
}

// TODO: Implement this
int Wrap_Texture::renderTo(lua_State* L)
{
    return 0;
}

int Wrap_Texture::getDebugName(lua_State* L)
{
    auto* self = luax_checktexture(L, 1);

    const auto& name = self->getDebugName();

    if (name.empty())
        lua_pushnil(L);
    else
        luax_pushstring(L, name);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getTextureType",     Wrap_Texture::getTextureType     },
    { "getWidth",           Wrap_Texture::getWidth           },
    { "getHeight",          Wrap_Texture::getHeight          },
    { "getDimensions",      Wrap_Texture::getDimensions      },
    { "getDepth",           Wrap_Texture::getDepth           },
    { "getLayerCount",      Wrap_Texture::getLayerCount      },
    { "getMipmapCount",     Wrap_Texture::getMipmapCount     },
    { "getPixelWidth",      Wrap_Texture::getPixelWidth      },
    { "getPixelHeight",     Wrap_Texture::getPixelHeight     },
    { "getPixelDimensions", Wrap_Texture::getPixelDimensions },
    { "getDPIScale",        Wrap_Texture::getDPIScale        },
    { "isFormatLinear",     Wrap_Texture::isFormatLinear     },
    { "isCompressed",       Wrap_Texture::isCompressed       },
    { "getMSAA",            Wrap_Texture::getMSAA            },
    { "setFilter",          Wrap_Texture::setFilter          },
    { "getFilter",          Wrap_Texture::getFilter          },
    { "setMipmapFilter",    Wrap_Texture::setMipmapFilter    },
    { "getMipmapFilter",    Wrap_Texture::getMipmapFilter    },
    { "setWrap",            Wrap_Texture::setWrap            },
    { "getWrap",            Wrap_Texture::getWrap            },
    { "getFormat",          Wrap_Texture::getFormat          },
    { "isCanvas",           Wrap_Texture::isCanvas           },
    { "isComputeWritable",  Wrap_Texture::isComputeWritable  },
    { "isReadable",         Wrap_Texture::isReadable         },
    { "getViewFormats",     Wrap_Texture::getViewFormats     },
    { "getMipmapMode",      Wrap_Texture::getMipmapMode      },
    { "getDepthSampleMode", Wrap_Texture::getDepthSampleMode },
    { "setDepthSampleMode", Wrap_Texture::setDepthSampleMode },
    { "generateMipmaps",    Wrap_Texture::generateMipmaps    },
    { "replacePixels",      Wrap_Texture::replacePixels      },
    { "renderTo",           Wrap_Texture::renderTo           },
    { "getDebugName",       Wrap_Texture::getDebugName       }
};
// clang-format on

namespace love
{
    TextureBase* luax_checktexture(lua_State* L, int index)
    {
        return luax_checktype<TextureBase>(L, index);
    }

    int open_texture(lua_State* L)
    {
        return luax_register_type(L, &Texture::type, functions);
    }
} // namespace love
