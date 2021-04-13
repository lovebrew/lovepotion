#pragma once

#include "deko3d/shader.h"
#include "common/luax.h"
namespace Wrap_Shader
{
    love::Shader* CheckShader(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Shader
