#include "objects/shader/wrap_shader.h"

using namespace love;

Shader* Wrap_Shader::CheckShader(lua_State* L, int index)
{
    return Luax::CheckType<Shader>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { 0, 0 }
};
// clang-format on

int Wrap_Shader::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Shader::type, functions, nullptr);
}
