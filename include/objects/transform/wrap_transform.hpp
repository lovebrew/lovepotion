#pragma once

#include <common/luax.hpp>
#include <objects/transform/transform.hpp>

namespace Wrap_Transform
{
    love::Transform* CheckTransform(lua_State* L, int index);

    int Clone(lua_State* L);

    int Inverse(lua_State* L);

    int Apply(lua_State* L);

    int IsAffine2DTransform(lua_State* L);

    int Translate(lua_State* L);

    int Rotate(lua_State* L);

    int Scale(lua_State* L);

    int Shear(lua_State* L);

    int Reset(lua_State* L);

    int SetTransformation(lua_State* L);

    int SetMatrix(lua_State* L);

    int GetMatrix(lua_State* L);

    int TransformPoint(lua_State* L);

    int InverseTransformPoint(lua_State* L);

    int __Mul(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Transform
