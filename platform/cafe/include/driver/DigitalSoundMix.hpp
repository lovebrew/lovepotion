#pragma once

#include <sndcore2/voice.h>

#define AX_VOICE(x)      x
#define AX_CHANNEL_LEFT  0
#define AX_CHANNEL_RIGHT 1
#define AX_BUS_MASTER    0

#define AX_NUM_CHANNELS 6

#define WIIU_MAX_VALID_CHANNELS 2

#define AX_VOICE_MONO   1
#define AX_VOICE_STEREO 2

// clang-format off
static AXVoiceDeviceMixData STEREO_MIX[2][AX_NUM_CHANNELS] =
{
    {  // AX_VOICE(0)
        {  // AX_CHANNEL_LEFT
            .bus = { { .volume = 0x8000 } }
        }
    },
    {  // AX_VOICE(1)
        {  // AX_CHANNEL_RIGHT
            .bus = { { .volume = 0x8000 } }
        }
    }
};

static AXVoiceDeviceMixData MONO_MIX[1][AX_NUM_CHANNELS] =
{
    {  // AX_VOICE(0)
        {  // AX_CHANNEL_LEFT
            .bus = { { .volume = 0x8000 } }
        },
        {  // AX_CHANNEL_RIGHT
            .bus = { { .volume = 0x8000 } }
        }
    }
};
// clang-format on
