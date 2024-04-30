#include "modules/image/wrap_ImageData.hpp"

#include "modules/data/wrap_Data.hpp"
#include "modules/filesystem/physfs/File.hpp"
#include "modules/filesystem/physfs/Filesystem.hpp"

using namespace love;

int Wrap_ImageData::clone(lua_State* L)
{
    auto* self       = luax_checkimagedata(L, 1);
    ImageData* clone = nullptr;

    luax_catchexcept(L, [&]() { clone = self->clone(); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_ImageData::getFormat(lua_State* L)
{
    auto* self  = luax_checkimagedata(L, 1);
    auto format = self->getFormat();

    std::string_view name;
    if (!love::getConstant(format, name))
        return luaL_error(L, "Unknown pixel format.");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_ImageData::setLinear(lua_State* L)
{
    auto* self  = luax_checkimagedata(L, 1);
    bool linear = luax_toboolean(L, 2);

    self->setLinear(linear);

    return 0;
}

int Wrap_ImageData::isLinear(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);

    luax_pushboolean(L, self->isLinear());

    return 1;
}

int Wrap_ImageData::getWidth(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);

    lua_pushinteger(L, self->getWidth());

    return 1;
}

int Wrap_ImageData::getHeight(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);

    lua_pushinteger(L, self->getHeight());

    return 1;
}

int Wrap_ImageData::getDimensions(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);

    lua_pushinteger(L, self->getWidth());
    lua_pushinteger(L, self->getHeight());

    return 2;
}

int Wrap_ImageData::getPixel(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);
    int x      = luaL_checkinteger(L, 2);
    int y      = luaL_checkinteger(L, 3);

    Color color {};
    luax_catchexcept(L, [&]() { self->getPixel(x, y, color); });

    lua_pushinteger(L, color.r);
    lua_pushinteger(L, color.g);
    lua_pushinteger(L, color.b);
    lua_pushinteger(L, color.a);

    return 4;
}

int Wrap_ImageData::setPixel(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);
    int x      = luaL_checkinteger(L, 2);
    int y      = luaL_checkinteger(L, 3);

    int components = love::getPixelFormatColorComponents(self->getFormat());

    Color color {};

    if (lua_istable(L, 4))
    {
        for (int index = 1; index <= components; index++)
            lua_rawgeti(L, 4, index);

        color.r = (float)luaL_checknumber(L, -components);
        if (components > 1)
            color.g = (float)luaL_checknumber(L, (-components) + 1);
        if (components > 2)
            color.b = (float)luaL_checknumber(L, (-components) + 2);
        if (components > 3)
            color.a = (float)luaL_optnumber(L, (-components) + 3, 1.0);

        lua_pop(L, components);
    }
    else
    {
        color.r = (float)luaL_checknumber(L, 4);
        if (components > 1)
            color.g = (float)luaL_checknumber(L, 5);
        if (components > 2)
            color.b = (float)luaL_checknumber(L, 6);
        if (components > 3)
            color.a = (float)luaL_optnumber(L, 7, 1.0);
    }

    luax_catchexcept(L, [&]() { self->setPixel(x, y, color); });

    return 0;
}

int Wrap_ImageData::mapPixel(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    int sx     = luaL_optinteger(L, 3, 0);
    int sy     = luaL_optinteger(L, 4, 0);
    int width  = luaL_optinteger(L, 5, self->getWidth());
    int height = luaL_optinteger(L, 6, self->getHeight());

    if (!(self->inside(sx, sy) && self->inside(sx + width - 1, sy + height - 1)))
        return luaL_error(L, "Invalid rectangle dimensions.");

    int imageWidth = self->getWidth();
    auto format    = self->getFormat();
    int components = love::getPixelFormatColorComponents(format);

    auto pixelSetFunction = self->getPixelSetFunction();
    auto pixelGetFunction = self->getPixelGetFunction();

    uint8_t* data    = (uint8_t*)self->getData();
    size_t pixelSize = self->getPixelSize();

    for (int y = sy; y < sy + height; y++)
    {
        for (int x = sx; x < sx + width; x++)
        {
            auto* pixel = (ImageData::Pixel*)(data + (y * imageWidth + x) * pixelSize);

            Color color {};
            pixelGetFunction(pixel, color);

            lua_pushvalue(L, 2); // ImageData

            lua_pushnumber(L, x);
            lua_pushnumber(L, y);

            lua_pushinteger(L, color.r);
            lua_pushinteger(L, color.g);
            lua_pushinteger(L, color.b);
            lua_pushinteger(L, color.a);

            lua_call(L, 6, 4);

            color.r = (float)luaL_checknumber(L, -4);

            if (components > 1)
                color.g = (float)luaL_checknumber(L, -3);

            if (components > 2)
                color.b = (float)luaL_checknumber(L, -2);

            if (components > 3)
                color.a = (float)luaL_optnumber(L, -1, 1.0);

            pixelSetFunction(color, pixel);
            lua_pop(L, 4);
        }
    }

    return 0;
}

int Wrap_ImageData::paste(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);
    auto* src  = luax_checkimagedata(L, 2);

    int dx = luaL_checkinteger(L, 3);
    int dy = luaL_checkinteger(L, 4);
    int sx = luaL_optinteger(L, 5, 0);
    int sy = luaL_optinteger(L, 6, 0);
    int sw = luaL_optinteger(L, 7, src->getWidth());
    int sh = luaL_optinteger(L, 8, src->getHeight());

    self->paste((ImageData*)src, dx, dy, sx, sy, sw, sh);

    return 0;
}

int Wrap_ImageData::encode(lua_State* L)
{
    auto* self = luax_checkimagedata(L, 1);

    FormatHandler::EncodedFormat format;
    const char* formatName = luaL_checkstring(L, 2);

    if (!ImageData::getConstant(formatName, format))
        return luax_enumerror(L, "encoded image format", ImageData::EncodedFormats, formatName);

    bool hasFilename = false;

    std::string filename = std::format("Image.{:s}", formatName);

    if (!lua_isnoneornil(L, 3))
    {
        hasFilename = true;
        filename    = luaL_checkstring(L, 3);
    }

    FileData* fileData = nullptr;
    luax_catchexcept(L, [&]() { fileData = self->encode(format, filename.c_str(), hasFilename); });

    luax_pushtype(L, fileData);
    fileData->release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",          Wrap_ImageData::clone         },
    { "getFormat",      Wrap_ImageData::getFormat     },
    { "setLinear",      Wrap_ImageData::setLinear     },
    { "isLinear",       Wrap_ImageData::isLinear      },
    { "getWidth",       Wrap_ImageData::getWidth      },
    { "getHeight",      Wrap_ImageData::getHeight     },
    { "getDimensions",  Wrap_ImageData::getDimensions },
    { "getPixel",       Wrap_ImageData::getPixel      },
    { "setPixel",       Wrap_ImageData::setPixel      },
    { "paste",          Wrap_ImageData::paste         },
    { "mapPixel",       Wrap_ImageData::mapPixel      },
    { "encode",         Wrap_ImageData::encode        }
};
// clang-format on

namespace love
{
    ImageData* luax_checkimagedata(lua_State* L, int index)
    {
        return luax_checktype<ImageData>(L, index);
    }

    int open_imagedata(lua_State* L)
    {
        return luax_register_type(L, &ImageData::type, Wrap_Data::functions, functions);
    }
} // namespace love
