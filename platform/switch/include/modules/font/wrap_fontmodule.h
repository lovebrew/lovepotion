#pragma once

#include "modules/font/fontmodule.h"

#include "objects/filedata/wrap_filedata.h"
#include "modules/filesystem/wrap_filesystem.h"

#include "objects/rasterizer/wrap_rasterizer.h"
#include "objects/glyphdata/wrap_glyphdata.h"

namespace Wrap_FontModule
{
    int NewRasterizer(lua_State * L);

    int NewGlyphData(lua_State * L);

    int NewTrueTypeRasterizer(lua_State * L);

    int Register(lua_State * L);
}