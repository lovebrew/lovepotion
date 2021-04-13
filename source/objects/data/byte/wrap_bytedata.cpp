#include "objects/data/byte/wrap_bytedata.h"

using namespace love;

ByteData * Wrap_ByteData::CheckByteData(lua_State * L, int index)
{
    return Luax::CheckType<ByteData>(L, index);
}

int Wrap_ByteData::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { 0, 0 }
    };

    return Luax::RegisterType(L, &ByteData::type, Wrap_Data::functions, reg, nullptr);
}
