#pragma once

#include "common/luax.hpp"
#include "modules/math/Transform.hpp"

namespace love
{
    Transform* luax_checktransform(lua_State* L, int index);

    void luax_checkmatrix(lua_State* L, int index, Transform::MatrixLayout layout, float elements[16]);

    int open_transform(lua_State* L);
} // namespace love

namespace Wrap_Transform
{
    int clone(lua_State* L);

    int inverse(lua_State* L);

    int apply(lua_State* L);

    int isAffine2DTransform(lua_State* L);

    int translate(lua_State* L);

    int rotate(lua_State* L);

    int scale(lua_State* L);

    int shear(lua_State* L);

    int reset(lua_State* L);

    int setTransformation(lua_State* L);

    int setMatrix(lua_State* L);

    int getMatrix(lua_State* L);

    int transformPoint(lua_State* L);

    int inverseTransformPoint(lua_State* L);

    int __mul(lua_State* L);
} // namespace Wrap_Transform
