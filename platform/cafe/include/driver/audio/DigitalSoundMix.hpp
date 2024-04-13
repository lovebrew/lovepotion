#pragma once

#include <sndcore2/voice.h>

#define AX_NUM_CHANNELS 6

#define AX_VOICE_MONO   1
#define AX_VOICE_STEREO 2

// clang-format off
constinit inline AXVoiceDeviceMixData STEREO_MIX[2][AX_NUM_CHANNELS] =
{
    { // AX_VOICE 0
        { // AX_CHANNEL_LEFT
            .bus = {{ .volume = 0x8000, .delta = 0 }}
        }
    },
    { // AX_VOICE 1
        { // AX_CHANNEL_RIGHT
            .bus = {{ .volume = 0x8000, .delta = 0 }}
        }
    }
};

constinit inline AXVoiceDeviceMixData MONO_MIX[1][AX_NUM_CHANNELS] =
{
    { // AX_VOICE 0
        { // AX_CHANNEL_LEFT
            .bus = {{ .volume = 0x8000, .delta = 0 }}
        },
        { // AX_CHANNEL_RIGHT
            .bus = {{ .volume = 0x8000, .delta = 0 }}
        }
    }
};
// clang-format on
