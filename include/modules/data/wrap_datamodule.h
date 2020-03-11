#pragma once

#include "modules/data/datamodule.h"

namespace Wrap_DataModule
{
    love::data::ContainerType CheckContainerType(lua_State * L, int index);

    int Register(lua_State * L);
}
