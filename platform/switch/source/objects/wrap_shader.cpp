#include "common/luax.h"
#include "objects/shader/wrap_shader.h"

using namespace love;

Shader * Wrap_Shader::CheckShader(lua_State * L, int index)
{
    return Luax::CheckType<Shader>(L, index);
}

int Wrap_Shader::Register(lua_State * L)
{
    luaL_Reg reg[] = {
        { 0, 0 }
    };

    return Luax::RegisterType(L, &Shader::type, reg, nullptr);
}