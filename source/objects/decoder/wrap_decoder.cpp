#include "common/runtime.h"
#include "objects/decoder/wrap_decoder.h"

using namespace love;

#define instance() (Module::GetInstance<Sound>(Module::M_SOUND))

Decoder * Wrap_Decoder::CheckDecoder(lua_State * L, int index)
{
    return Luax::CheckType<Decoder>(L, index);
}

int Wrap_Decoder::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { 0, 0 }
    };

    return Luax::RegisterType(L, &Decoder::type, reg, nullptr);
}
