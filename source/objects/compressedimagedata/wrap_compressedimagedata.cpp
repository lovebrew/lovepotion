#include "objects/compressedimagedata/wrap_compressedimagedata.h"
#include "objects/compressedimagedata/compressedimagedata.h"

#include "modules/image/imagemodule.h"
#include "objects/data/wrap_data.h"

using namespace love;

int Wrap_CompressedImageData::Clone(lua_State* L)
{
    CompressedImageData* self  = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);
    CompressedImageData* clone = nullptr;

    Luax::CatchException(L, [&]() { clone = self->Clone(); });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_CompressedImageData::GetWidth(lua_State* L)
{
    CompressedImageData* self = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);

    int mipLevel = luaL_optinteger(L, 2, 1);

    int width = 0;
    Luax::CatchException(L, [&]() { width = self->GetWidth(mipLevel + 1); });

    lua_pushinteger(L, width);

    return 1;
}

int Wrap_CompressedImageData::GetHeight(lua_State* L)
{
    CompressedImageData* self = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);

    int mipLevel = luaL_optinteger(L, 2, 1);

    int height = 0;
    Luax::CatchException(L, [&]() { height = self->GetHeight(mipLevel + 1); });

    lua_pushinteger(L, height);

    return 1;
}

int Wrap_CompressedImageData::GetDimensions(lua_State* L)
{
    CompressedImageData* self = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);

    int mipLevel = luaL_optinteger(L, 2, 1);

    int height = 0;
    int width  = 0;

    Luax::CatchException(L, [&]() {
        width  = self->GetWidth(mipLevel + 1);
        height = self->GetHeight(mipLevel + 1);
    });

    lua_pushinteger(L, width);
    lua_pushinteger(L, height);

    return 2;
}

int Wrap_CompressedImageData::GetMipmapCount(lua_State* L)
{
    CompressedImageData* self = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);

    lua_pushinteger(L, self->GetMipmapCount());

    return 1;
}

int Wrap_CompressedImageData::GetFormat(lua_State* L)
{
    CompressedImageData* self = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);

    PixelFormat format = self->GetFormat();
    const char* string = nullptr;

    if (ImageModule::GetConstant(format, string))
        lua_pushstring(L, string);
    else
        lua_pushstring(L, "unknown");

    return 1;
}

CompressedImageData* Wrap_CompressedImageData::CheckCompressedImageData(lua_State* L, int index)
{
    return Luax::CheckType<CompressedImageData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",          Wrap_CompressedImageData::Clone          },
    { "getWidth",       Wrap_CompressedImageData::GetWidth       },
    { "getHeight",      Wrap_CompressedImageData::GetHeight      },
    { "getDimensions",  Wrap_CompressedImageData::GetDimensions  },
    { "getMipmapCount", Wrap_CompressedImageData::GetMipmapCount },
    { "getFormat",      Wrap_CompressedImageData::GetFormat      },
    { 0,                0                                        }
};
// clang-format on

int Wrap_CompressedImageData::Register(lua_State* L)
{
    return Luax::RegisterType(L, &CompressedImageData::type, Wrap_Data::functions, functions,
                              nullptr);
}
