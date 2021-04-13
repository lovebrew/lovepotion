#pragma once

#include "objects/data/view/dataview.h"
#include "objects/data/wrap_data.h"

namespace Wrap_DataView
{
    love::DataView* CheckDataView(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_DataView
