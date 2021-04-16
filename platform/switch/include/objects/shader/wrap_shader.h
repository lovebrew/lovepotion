#pragma once

#include "common/luax.h"
#include "deko3d/shader.h"
namespace Wrap_Shader
{
    love::Shader* CheckShader(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Shader
