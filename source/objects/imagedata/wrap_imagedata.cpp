#include "objects/imagedata/wrap_imagedata.h"

#include "modules/filesystem/filesystem.h"
#include "modules/image/imagemodule.h"

#include "common/pixelformat.h"

#include "objects/data/wrap_data.h"
#include "objects/file/file.h"

#include "common/lmath.h"

#include "wrap_imagedata_lua.h"

#include "modules/thread/types/lock.h"

using namespace love;

int Wrap_ImageData::Clone(lua_State* L)
{
    ImageData* self  = Wrap_ImageData::CheckImageData(L, 1);
    ImageData* clone = nullptr;

    Luax::CatchException(L, [&]() { clone = self->Clone(); });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_ImageData::GetFormat(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);

    PixelFormat format    = self->GetFormat();
    const char* formatStr = nullptr;

    if (!ImageModule::GetConstant(format, formatStr))
        return luaL_error(L, "Unknown pixel format.");

    lua_pushstring(L, formatStr);

    return 1;
}

int Wrap_ImageData::GetWidth(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);

    lua_pushinteger(L, self->GetWidth());

    return 1;
}

int Wrap_ImageData::GetHeight(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);

    lua_pushinteger(L, self->GetHeight());

    return 1;
}

int Wrap_ImageData::GetDimensions(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);

    lua_pushinteger(L, self->GetWidth());
    lua_pushinteger(L, self->GetHeight());

    return 2;
}

int Wrap_ImageData::GetPixel(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    Colorf color {};
    Luax::CatchException(L, [&]() { self->GetPixel(x, y, color); });

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_ImageData::SetPixel(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    int components = love::GetPixelFormatColorComponents(self->GetFormat());
    Colorf color {};

    if (lua_istable(L, 4))
    {
        for (int index = 1; index <= components; index++)
            lua_rawgeti(L, components, index);

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

    Luax::CatchException(L, [&]() { self->SetPixel(x, y, color); });

    return 0;
}

// ImageData:mapPixel. Not thread-safe! See wrap_ImageData.lua for the thread-
// safe wrapper function.
int Wrap_ImageData::_MapPixelUnsafe(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    // No optints because we assume they're done in the wrapper function.
    int sourceX = (int)lua_tonumber(L, 3);
    int sourceY = (int)lua_tonumber(L, 4);
    int width   = (int)lua_tonumber(L, 5);
    int height  = (int)lua_tonumber(L, 6);

    if (!(self->Inside(sourceX, sourceY) &&
          self->Inside(sourceX + width - 1, sourceY + height - 1)))
        return luaL_error(L, "Invalid rectangle dimensions.");

    int imageWidth = self->GetWidth();

    PixelFormat format = self->GetFormat();
    int components     = love::GetPixelFormatColorComponents(format);

    auto pixelsetfunction = self->GetPixelSetFunction(format);
    auto pixelgetfunction = self->GetPixelGetFunction(format);

#if defined(__3DS__)
    unsigned _srcPowTwo = NextPO2(imageWidth + 2);
    uint32_t* data      = reinterpret_cast<uint32_t*>(self->GetData());

    for (int y = sourceY; y < sourceY + height; y++)
    {
        for (int x = sourceX; x < sourceX + width; x++)
        {
            unsigned index = coordToIndex(_srcPowTwo, x + 1, y + 1);
            auto pixel     = reinterpret_cast<ImageData::Pixel*>(data + index);

            Colorf color {};
            pixelgetfunction(pixel, color);

            lua_pushvalue(L, 2); // ImageData

            lua_pushnumber(L, x);
            lua_pushnumber(L, y);

            lua_pushnumber(L, color.r);
            lua_pushnumber(L, color.g);
            lua_pushnumber(L, color.b);
            lua_pushnumber(L, color.a);

            lua_call(L, 6, 4);

            color.r = (float)luaL_checknumber(L, -4);
            if (components > 1)
                color.g = (float)luaL_checknumber(L, -3);
            if (components > 2)
                color.b = (float)luaL_checknumber(L, -2);
            if (components > 3)
                color.a = (float)luaL_optnumber(L, -1, 1.0);

            pixelsetfunction(color, pixel);

            lua_pop(L, 4); // Pop return values.
        }
    }
#elif defined(__SWITCH__)
    uint8_t* data    = (uint8_t*)self->GetData();
    size_t pixelsize = self->GetPixelSize();

    for (int y = sourceY; y < sourceY + height; y++)
    {
        for (int x = sourceX; x < sourceX + width; x++)
        {
            auto pixeldata = (ImageData::Pixel*)(data + (y * imageWidth + x) * pixelsize);

            Colorf color {};
            pixelgetfunction(pixeldata, color);

            lua_pushvalue(L, 2); // ImageData

            lua_pushnumber(L, x);
            lua_pushnumber(L, y);

            lua_pushnumber(L, color.r);
            lua_pushnumber(L, color.g);
            lua_pushnumber(L, color.b);
            lua_pushnumber(L, color.a);

            lua_call(L, 6, 4);

            color.r = (float)luaL_checknumber(L, -4);
            if (components > 1)
                color.g = (float)luaL_checknumber(L, -3);
            if (components > 2)
                color.b = (float)luaL_checknumber(L, -2);
            if (components > 3)
                color.a = (float)luaL_optnumber(L, -1, 1.0);

            pixelsetfunction(color, pixeldata);

            lua_pop(L, 4); // Pop return values.
        }
    }
#endif
    return 0;
}

int Wrap_ImageData::Paste(lua_State* L)
{
    ImageData* t   = Wrap_ImageData::CheckImageData(L, 1);
    ImageData* src = Wrap_ImageData::CheckImageData(L, 2);

    int destinationX = (int)luaL_checkinteger(L, 3);
    int destinationY = (int)luaL_checkinteger(L, 4);

    int sourceX = (int)luaL_optinteger(L, 5, 0);
    int sourceY = (int)luaL_optinteger(L, 6, 0);
    int sourceW = (int)luaL_optinteger(L, 7, src->GetWidth());
    int sourceH = (int)luaL_optinteger(L, 8, src->GetHeight());

    t->Paste((ImageData*)src, destinationX, destinationY, sourceX, sourceY, sourceW, sourceH);

    return 0;
}

int Wrap_ImageData::Encode(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);

    FormatHandler::EncodedFormat format;
    const char* formatStr = luaL_checkstring(L, 2);

    if (!ImageData::GetConstant(formatStr, format))
        return Luax::EnumError(L, "encoded image format", ImageData::GetConstants(format),
                               formatStr);

    bool hasFilename     = false;
    std::string filename = "Image." + std::string(formatStr);

    if (!lua_isnoneornil(L, 3))
    {
        hasFilename = true;
        filename    = Luax::CheckString(L, 3);
    }

    FileData* fileData = nullptr;
    Luax::CatchException(L,
                         [&]() { fileData = self->Encode(format, filename.c_str(), hasFilename); });

    Luax::PushType(L, fileData);
    fileData->Release();

    return 1;
}

int Wrap_ImageData::_PerformAtomic(lua_State* L)
{
    ImageData* self = Wrap_ImageData::CheckImageData(L, 1);
    int error       = 0;

    {
        thread::Lock lock(self->GetMutex());

        // call the function, passing any user-specified arguments.
        error = lua_pcall(L, lua_gettop(L) - 2, LUA_MULTRET, 0);
    }

    if (error != 0)
        return lua_error(L);

    /* The function and everything after it in the stack are eaten by the pcall */
    /* leaving only the ImageData object. Everything else is a return value. */
    return lua_gettop(L) - 1;
}

ImageData* Wrap_ImageData::CheckImageData(lua_State* L, int index)
{
    return Luax::CheckType<ImageData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",           Wrap_ImageData::Clone           },
    { "getFormat",       Wrap_ImageData::GetFormat       },
    { "getWidth",        Wrap_ImageData::GetWidth        },
    { "getHeight",       Wrap_ImageData::GetHeight       },
    { "getDimensions",   Wrap_ImageData::GetDimensions   },
    { "getPixel",        Wrap_ImageData::GetPixel        },
    { "setPixel",        Wrap_ImageData::SetPixel        },
    { "paste",           Wrap_ImageData::Paste           },
    { "encode",          Wrap_ImageData::Encode          },
    { "_mapPixelUnsafe", Wrap_ImageData::_MapPixelUnsafe },
    { "_performAtomic",  Wrap_ImageData::_PerformAtomic  },
    { 0,                 0                               }
};
// clang-format on

int Wrap_ImageData::Register(lua_State* L)
{
    int result = Luax::RegisterType(L, &ImageData::type, Wrap_Data::functions, functions, nullptr);

    Luax::RunWrapper(L, (const char*)wrap_imagedata_lua, wrap_imagedata_lua_size,
                     "wrap_imagedata.lua", ImageData::type);

    return result;
}
