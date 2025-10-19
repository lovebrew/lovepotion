#include "modules/audio/wrap_RecordingDevice.hpp"
#include "modules/audio/wrap_Audio.hpp"

using namespace love;

int Wrap_RecordingDevice::start(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);

    int samples    = self->getMaxSamples();
    int sampleRate = self->getSampleRate();
    int bitDepth   = self->getBitDepth();
    int channels   = self->getChannelCount();

    if (lua_gettop(L) > 1)
    {
        samples    = luaL_checkinteger(L, 2);
        sampleRate = luaL_optinteger(L, 3, RecordingDeviceBase::DEFAULT_SAMPLE_RATE);
        bitDepth   = luaL_optinteger(L, 4, RecordingDeviceBase::DEFAULT_BIT_DEPTH);
        channels   = luaL_optinteger(L, 5, RecordingDeviceBase::DEFAULT_CHANNELS);
    }

    bool success = false;
    luax_catchexcept(L, [&]() { success = self->start(samples, sampleRate, bitDepth, channels); });

    luax_pushboolean(L, success);

    return 1;
}

int Wrap_RecordingDevice::stop(lua_State* L)
{
    auto* self      = luax_checkrecordingdevice(L, 1);
    SoundData* data = nullptr;

    luax_catchexcept(L, [&]() { data = self->getData(); });
    self->stop();

    if (data)
    {
        luax_pushtype(L, data);
        data->release();
    }
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_RecordingDevice::getData(lua_State* L)
{
    auto* self      = luax_checkrecordingdevice(L, 1);
    SoundData* data = nullptr;

    luax_catchexcept(L, [&]() { data = self->getData(); });

    if (data)
    {
        luax_pushtype(L, data);
        data->release();
    }
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_RecordingDevice::getSampleCount(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);
    lua_pushnumber(L, self->getSampleCount());

    return 1;
}

int Wrap_RecordingDevice::getSampleRate(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);
    lua_pushnumber(L, self->getSampleRate());

    return 1;
}

int Wrap_RecordingDevice::getBitDepth(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);
    lua_pushnumber(L, self->getBitDepth());

    return 1;
}

int Wrap_RecordingDevice::getChannelCount(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);
    lua_pushnumber(L, self->getChannelCount());

    return 1;
}

int Wrap_RecordingDevice::getName(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);
    luax_pushstring(L, self->getName());

    return 1;
}

int Wrap_RecordingDevice::isRecording(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);
    lua_pushboolean(L, self->isRecording());

    return 1;
}

int Wrap_RecordingDevice::setGain(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);
    int gain   = luaL_checkinteger(L, 2);

    if (gain != gain)
        return luaL_error(L, "Gain cannot be NaN.");

    if (gain > 63 || gain <= 0)
        return luaL_error(L, "Gain must be in the range [0, 63].");

    luax_catchexcept(L, [&]() { self->setGain(static_cast<uint8_t>(gain)); });

    return 0;
}

int Wrap_RecordingDevice::getGain(lua_State* L)
{
    auto* self = luax_checkrecordingdevice(L, 1);
    lua_pushnumber(L, self->getGain());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "start",          Wrap_RecordingDevice::start           },
    { "stop",           Wrap_RecordingDevice::stop            },
    { "getData",        Wrap_RecordingDevice::getData         },
    { "getSampleCount", Wrap_RecordingDevice::getSampleCount  },
    { "getSampleRate",  Wrap_RecordingDevice::getSampleRate   },
    { "getBitDepth",    Wrap_RecordingDevice::getBitDepth     },
    { "getChannelCount",Wrap_RecordingDevice::getChannelCount },
    { "getName",        Wrap_RecordingDevice::getName         },
    { "isRecording",    Wrap_RecordingDevice::isRecording     },
    { "setGain",        Wrap_RecordingDevice::setGain         },
    { "getGain",        Wrap_RecordingDevice::getGain         }
};
// clang-format on

namespace love
{
    RecordingDeviceBase* luax_checkrecordingdevice(lua_State* L, int index)
    {
        return luax_checktype<RecordingDeviceBase>(L, index, RecordingDeviceBase::type);
    }

    int open_recordingdevice(lua_State* L)
    {
        return luax_register_type(L, &RecordingDeviceBase::type, functions);
    }
} // namespace love
