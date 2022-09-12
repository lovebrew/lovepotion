#pragma once

#include <common/luax.hpp>

#include <objects/data/dataview/dataview.hpp>
#include <objects/data/wrap_data.hpp>

namespace Wrap_DataView
{
    love::DataView* CheckDataView(lua_State* L, int index);

    int Clone(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_DataView
