#include <objects/imagedata/wrap_imagedata.hpp>

using namespace love;
using ImageData = love::ImageData<Console::Which>;

int Wrap_ImageData::__MapPixelUnsafe(lua_State* L)
{
    auto* self = Wrap_ImageData::CheckImageData(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    int sourceX = lua_tonumber(L, 3);
    int sourceY = lua_tonumber(L, 4);
    int width   = lua_tonumber(L, 5);
    int height  = lua_tonumber(L, 6);

    if (!(self->Inside(sourceX, sourceY) &&
          self->Inside(sourceX + width - 1, sourceY + height - 1)))
    {
        return luaL_error(L, "Invalid rectangle dimensions.");
    }

    int imageWidth = self->GetWidth();

    PixelFormat format = self->GetFormat();
    int components     = love::GetPixelFormatColorComponents(format);

    auto pixelSetFunction = self->GetPixelSetFunction();
    auto pixelGetFunction = self->GetPixelGetFunction();

    uint8_t* data    = (uint8_t*)self->GetData();
    size_t pixelSize = self->GetPixelSize();

    for (int y = sourceY; y < sourceY + height; y++)
    {
        for (int x = sourceX; x < sourceX + width; x++)
        {
            auto pixelData = (::ImageData::Pixel*)(data + (y * imageWidth + x) * pixelSize);

            Color color {};
            pixelGetFunction(pixelData, color);

            lua_pushvalue(L, 2);

            lua_pushnumber(L, x);
            lua_pushnumber(L, y);

            lua_pushnumber(L, color.r);
            lua_pushnumber(L, color.g);
            lua_pushnumber(L, color.b);
            lua_pushnumber(L, color.a);

            lua_call(L, 6, 4);

            color.r = luaL_checknumber(L, -4);

            if (components > 1)
                color.g = luaL_checknumber(L, -3);

            if (components > 2)
                color.b = luaL_checknumber(L, -2);

            if (components > 3)
                color.a = luaL_checknumber(L, -1);

            pixelSetFunction(color, pixelData);

            lua_pop(L, 4);
        }
    }

    return 0;
}

// clang-format off
static constexpr luaL_Reg extensions[] =
{
    { "_mapPixelUnsafe", Wrap_ImageData::__MapPixelUnsafe }
};
// clang-forma ton

std::span<const luaL_Reg> Wrap_ImageData::extensions = extensions;
