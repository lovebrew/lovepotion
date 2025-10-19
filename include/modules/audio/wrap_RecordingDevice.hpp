#pragma once

#include "common/luax.hpp"
#include "modules/audio/RecordingDevice.tcc"

namespace love
{
    RecordingDeviceBase* luax_checkrecordingdevice(lua_State* L, int index);

    int open_recordingdevice(lua_State* L);
} // namespace love

namespace Wrap_RecordingDevice
{
    int start(lua_State* L);

    int stop(lua_State* L);

    int getData(lua_State* L);

    int getName(lua_State* L);

    int getSampleCount(lua_State* L);

    int getSampleRate(lua_State* L);

    int getBitDepth(lua_State* L);

    int getChannelCount(lua_State* L);

    int isRecording(lua_State* L);

    int setGain(lua_State* L);

    int getGain(lua_State* L);
} // namespace Wrap_RecordingDevice
