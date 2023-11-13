#include <objects/compressedimagedata/wrap_compressedimagedata.hpp>

#include <objects/data/wrap_data.hpp>

using namespace love;

CompressedImageData* Wrap_CompressedImageData::CheckCompressedImageData(lua_State* L, int index)
{
    return luax::CheckType<CompressedImageData>(L, index);
}

int Wrap_CompressedImageData::Clone(lua_State* L)
{
    auto* self                 = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);
    CompressedImageData* clone = nullptr;

    luax::CatchException(L, [&]() { clone = self->Clone(); });
    luax::PushType(L, clone);

    clone->Release();

    return 1;
}

int Wrap_CompressedImageData::GetWidth(lua_State* L)
{
    auto* self   = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);
    int miplevel = (int)luaL_optinteger(L, 2, 1);
    int width    = 0;

    luax::CatchException(L, [&]() { width = self->GetWidth(miplevel - 1); });

    lua_pushinteger(L, width);

    return 1;
}

int Wrap_CompressedImageData::GetHeight(lua_State* L)
{
    auto* self   = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);
    int miplevel = (int)luaL_optinteger(L, 2, 1);
    int height   = 0;

    luax::CatchException(L, [&]() { height = self->GetHeight(miplevel - 1); });

    lua_pushinteger(L, height);

    return 1;
}

int Wrap_CompressedImageData::GetDimensions(lua_State* L)
{
    auto* self   = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);
    int miplevel = luaL_optinteger(L, 2, 1);

    int width  = 0;
    int height = 0;

    luax::CatchException(L, [&]() {
        width  = self->GetWidth(miplevel - 1);
        height = self->GetHeight(miplevel - 1);
    });

    lua_pushinteger(L, width);
    lua_pushinteger(L, height);

    return 2;
}

int Wrap_CompressedImageData::GetMipmapCount(lua_State* L)
{
    auto* self = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);

    lua_pushinteger(L, self->GetMipmapCount());

    return 1;
}

int Wrap_CompressedImageData::GetFormat(lua_State* L)
{
    auto* self = Wrap_CompressedImageData::CheckCompressedImageData(L, 1);

    const auto format  = self->GetFormat();
    const char* string = love::GetPixelFormatName(format);

    lua_pushstring(L, string);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",          Wrap_CompressedImageData::Clone          },
    { "getWidth",       Wrap_CompressedImageData::GetWidth       },
    { "getHeight",      Wrap_CompressedImageData::GetHeight      },
    { "getDimensions",  Wrap_CompressedImageData::GetDimensions  },
    { "getMipmapCount", Wrap_CompressedImageData::GetMipmapCount },
    { "getFormat",      Wrap_CompressedImageData::GetFormat      }
};
// clang-format on

int Wrap_CompressedImageData::Register(lua_State* L)
{
    return luax::RegisterType(L, &CompressedImageData::type, functions, Wrap_Data::functions);
}
