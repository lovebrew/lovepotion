#pragma once

#include "common/luax.h"

#include "objects/decoder/decoder.h"
#include "objects/decoder/wrap_decoder.h"

#include "objects/sounddata/wrap_sounddata.h"

#include "modules/filesystem/wrap_filesystem.h"
#include "modules/sound/sound.h"

namespace Wrap_Sound
{
    int NewDecoder(lua_State* L);

    int NewSoundData(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Sound
