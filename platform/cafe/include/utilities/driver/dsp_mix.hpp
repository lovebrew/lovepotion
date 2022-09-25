#pragma once

#include <sndcore2/voice.h>

#define AX_VOICE(x)      x
#define AX_CHANNEL_LEFT  0
#define AX_CHANNEL_RIGHT 1
#define AX_BUS_MASTER    0
#define AX_NUM_CHANNELS  6

#define MIX_MONO   1
#define MIX_STEREO 2

// clang-format off
static AXVoiceDeviceMixData STEREO_MIX[MIX_STEREO][AX_NUM_CHANNELS] = {
    {
        { .bus = { { .volume = 0x8000 } } }, //< AX_CHANNEL_LEFT
    },
    {
        { .bus = { { .volume = 0x8000 } } }  //< AX_CHANNEL_RIGHT
    }
};

static AXVoiceDeviceMixData MONO_MIX[MIX_MONO][AX_NUM_CHANNELS] =
{
    {
        { .bus = { { .volume = 0x8000 } } }, //< AX_CHANNEL_LEFT
        { .bus = { { .volume = 0x8000 } } }  //< AX_CHANNEL_RIGHT
    }
};
// clang-format on
