#include "modules/data/wrap_CompressedData.hpp"
#include "modules/data/wrap_Data.hpp"

using namespace love;

int Wrap_CompressedData::clone(lua_State* L)
{
    auto* self            = luax_checkcompresseddata(L, 1);
    CompressedData* clone = nullptr;

    luax_catchexcept(L, [&] { clone = self->clone(); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_CompressedData::getFormat(lua_State* L)
{
    auto* self  = luax_checkcompresseddata(L, 1);
    auto format = self->getFormat();

    std::string_view name {};
    if (!Compressor::getConstant(format, name))
        return luax_enumerror(L, "compressed data format", Compressor::formats, name);

    luax_pushstring(L, name);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",     Wrap_CompressedData::clone     },
    { "getFormat", Wrap_CompressedData::getFormat }
};
// clang-format on

namespace love
{
    CompressedData* luax_checkcompresseddata(lua_State* L, int index)
    {
        return luax_checktype<CompressedData>(L, index);
    }

    int open_compresseddata(lua_State* L)
    {
        return luax_register_type(L, &CompressedData::type, Wrap_Data::functions, functions);
    }
} // namespace love
