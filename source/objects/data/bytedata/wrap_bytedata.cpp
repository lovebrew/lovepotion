#include "objects/data/bytedata/wrap_bytedata.hpp"

using namespace love;

int Wrap_ByteData::Clone(lua_State* L)
{
    ByteData* self  = Wrap_ByteData::CheckByteData(L, 1);
    ByteData* clone = nullptr;

    luax::CatchException(L, [&]() { clone = self->Clone(); });

    luax::PushType(L, clone);
    clone->Release();

    return 1;
}

ByteData* Wrap_ByteData::CheckByteData(lua_State* L, int index)
{
    return luax::CheckType<ByteData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone", Wrap_ByteData::Clone }
};
// clang-format on

int Wrap_ByteData::Register(lua_State* L)
{
    return luax::RegisterType(L, &ByteData::type, Wrap_Data::functions, functions);
}
