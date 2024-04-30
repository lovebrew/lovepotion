#include "modules/image/wrap_CompressedImageData.hpp"
#include "modules/data/wrap_Data.hpp"

using namespace love;

int Wrap_CompressedImageData::clone(lua_State* L)
{
    auto* self                 = luax_checkcompressedimagedata(L, 1);
    CompressedImageData* clone = nullptr;

    luax_catchexcept(L, [&]() { clone = self->clone(); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_CompressedImageData::getWidth(lua_State* L)
{
    auto* self = luax_checkcompressedimagedata(L, 1);

    int mipmap = (int)luaL_optinteger(L, 2, 1);
    int width  = 0;

    luax_catchexcept(L, [&]() { width = self->getWidth(mipmap - 1); });

    lua_pushinteger(L, width);

    return 1;
}

int Wrap_CompressedImageData::getHeight(lua_State* L)
{
    auto* self = luax_checkcompressedimagedata(L, 1);

    int mipmap = (int)luaL_optinteger(L, 2, 1);
    int height = 0;

    luax_catchexcept(L, [&]() { height = self->getHeight(mipmap - 1); });

    lua_pushinteger(L, height);

    return 1;
}

int Wrap_CompressedImageData::getDimensions(lua_State* L)
{
    auto* self = luax_checkcompressedimagedata(L, 1);

    int mipmap = (int)luaL_optinteger(L, 2, 1);
    int width  = 0;
    int height = 0;

    luax_catchexcept(L, [&]() {
        width  = self->getWidth(mipmap - 1);
        height = self->getHeight(mipmap - 1);
    });

    lua_pushinteger(L, width);
    lua_pushinteger(L, height);

    return 2;
}

int Wrap_CompressedImageData::getMipmapCount(lua_State* L)
{
    auto* self = luax_checkcompressedimagedata(L, 1);

    lua_pushinteger(L, self->getMipmapCount());

    return 1;
}

int Wrap_CompressedImageData::getFormat(lua_State* L)
{
    auto* self = luax_checkcompressedimagedata(L, 1);

    auto format = self->getFormat();
    std::string_view name;

    if (getConstant(format, name))
        luax_pushstring(L, name);
    else
        lua_pushstring(L, "unknown");

    return 1;
}

int Wrap_CompressedImageData::setLinear(lua_State* L)
{
    auto* self  = luax_checkcompressedimagedata(L, 1);
    bool linear = luax_toboolean(L, 2);

    self->setLinear(linear);

    return 0;
}

int Wrap_CompressedImageData::isLinear(lua_State* L)
{
    auto* self = luax_checkcompressedimagedata(L, 1);

    lua_pushboolean(L, self->isLinear());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",          Wrap_CompressedImageData::clone          },
    { "getWidth",       Wrap_CompressedImageData::getWidth       },
    { "getHeight",      Wrap_CompressedImageData::getHeight      },
    { "getDimensions",  Wrap_CompressedImageData::getDimensions  },
    { "getMipmapCount", Wrap_CompressedImageData::getMipmapCount },
    { "getFormat",      Wrap_CompressedImageData::getFormat      },
    { "setLinear",      Wrap_CompressedImageData::setLinear      },
    { "isLinear",       Wrap_CompressedImageData::isLinear       }
};
// clang-format on

namespace love
{
    CompressedImageData* luax_checkcompressedimagedata(lua_State* L, int index)
    {
        return luax_checktype<CompressedImageData>(L, index);
    }

    int open_compressedimagedata(lua_State* L)
    {
        return luax_register_type(L, &CompressedImageData::type, Wrap_Data::functions, functions);
    }
} // namespace love
