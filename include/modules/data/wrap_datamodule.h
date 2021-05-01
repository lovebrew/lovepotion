#pragma once

#include "common/luax.h"
#include "modules/data/datamodule.h"

#include "objects/data/wrap_data.h"

#include "objects/data/byte/bytedata.h"
#include "objects/data/byte/wrap_bytedata.h"

#include "objects/data/compressed/compresseddata.h"
#include "objects/data/compressed/wrap_compresseddata.h"

#include "objects/data/view/dataview.h"
#include "objects/data/view/wrap_dataview.h"
#include <limits>

namespace Wrap_DataModule
{
    love::data::ContainerType CheckContainerType(lua_State* L, int index);

    int NewByteData(lua_State* L);

    int NewDataView(lua_State* L);

    int Hash(lua_State* L);

    int Compress(lua_State* L);

    int Decompress(lua_State* L);

    int Decode(lua_State* L);

    int Encode(lua_State* L);

    int Pack(lua_State* L);

    int Unpack(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_DataModule
