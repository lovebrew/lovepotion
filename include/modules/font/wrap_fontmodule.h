#pragma once

#include "modules/font/fontmodule.h"

#include "modules/filesystem/wrap_filesystem.h"
#include "objects/filedata/wrap_filedata.h"

#include "objects/glyphdata/wrap_glyphdata.h"
#include "objects/rasterizer/wrap_rasterizer.h"

namespace Wrap_FontModule
{
    int NewRasterizer(lua_State* L);

    int NewGlyphData(lua_State* L);

    int NewTrueTypeRasterizer(lua_State* L);

    int NewBCFNTRasterizer(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_FontModule
