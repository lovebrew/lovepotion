#include "objects/data/compressed/wrap_compresseddata.h"

using namespace love;

int Wrap_CompressedData::Clone(lua_State* L)
{
    CompressedData* compressedData = Wrap_CompressedData::CheckCompressedData(L, 1);
    CompressedData* clone          = nullptr;

    Luax::CatchException(L, [&]() { clone = compressedData->Clone(); });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_CompressedData::GetFormat(lua_State* L)
{
    CompressedData* compressedData = Wrap_CompressedData::CheckCompressedData(L, 1);
    const char* formatName         = nullptr;

    if (!Compressor::GetConstant(compressedData->GetFormat(), formatName))
        return Luax::EnumError(L, "compressed data format",
                               Compressor::GetConstants(Compressor::FORMAT_MAX_ENUM), formatName);

    lua_pushstring(L, formatName);

    return 1;
}

CompressedData* Wrap_CompressedData::CheckCompressedData(lua_State* L, int index)
{
    return Luax::CheckType<CompressedData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",     Wrap_CompressedData::Clone     },
    { "getFormat", Wrap_CompressedData::GetFormat },
    { 0,           0                              }
};
// clang-format on

int Wrap_CompressedData::Register(lua_State* L)
{
    return Luax::RegisterType(L, &CompressedData::type, Wrap_Data::functions, functions, nullptr);
}
