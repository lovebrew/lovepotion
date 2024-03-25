#pragma once

#include "common/luax.hpp"
#include "modules/data/DataView.hpp"

namespace love
{
    DataView* luax_checkdataview(lua_State* L, int index);

    int open_dataview(lua_State* L);
} // namespace love

namespace Wrap_DataView
{
    int clone(lua_State* L);
}
