#include "objects/data/byte/wrap_bytedata.h"

using namespace love;

int Wrap_ByteData::Clone(lua_State* L)
{
    ByteData* self  = Wrap_ByteData::CheckByteData(L, 1);
    ByteData* clone = nullptr;

    Luax::CatchException(L, [&]() { clone = self->Clone(); });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

ByteData* Wrap_ByteData::CheckByteData(lua_State* L, int index)
{
    return Luax::CheckType<ByteData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone", Wrap_ByteData::Clone },
    { 0,       0                    }
};
// clang-format on

int Wrap_ByteData::Register(lua_State* L)
{
    return Luax::RegisterType(L, &ByteData::type, Wrap_Data::functions, functions, nullptr);
}
