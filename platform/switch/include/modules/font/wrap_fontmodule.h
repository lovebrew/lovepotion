#pragma once

#include "modules/font/fontmodule.h"

#include "objects/filedata/wrap_filedata.h"
#include "modules/filesystem/wrap_filesystem.h"

namespace Wrap_FontModule
{
    int NewRasterizer(lua_State * L);

    int NewGlyphData(lua_State * L);

    int NewFontRasterizer(lua_State * L);

    int Register(lua_State * L);
}