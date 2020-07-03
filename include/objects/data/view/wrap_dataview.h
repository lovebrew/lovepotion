#pragma once

#include "objects/data/wrap_data.h"
#include "objects/data/view/dataview.h"

namespace Wrap_DataView
{
    love::DataView * CheckDataView(lua_State * L, int index);

    int Register(lua_State * L);
}
