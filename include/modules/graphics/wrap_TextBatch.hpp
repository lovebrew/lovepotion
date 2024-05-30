#pragma once

#include "common/luax.hpp"
#include "modules/graphics/TextBatch.hpp"

namespace love
{
    TextBatch* luax_checktextbatch(lua_State* L, int index);

    int open_textbatch(lua_State* L);
} // namespace love

namespace Wrap_TextBatch
{
    int set(lua_State* L);

    int setf(lua_State* L);

    int add(lua_State* L);

    int addf(lua_State* L);

    int clear(lua_State* L);

    int setFont(lua_State* L);

    int getFont(lua_State* L);

    int getWidth(lua_State* L);

    int getHeight(lua_State* L);

    int getDimensions(lua_State* L);
} // namespace Wrap_TextBatch
