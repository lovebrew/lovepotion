#pragma once

#include <sndcore2/voice.h>

#include <cstring>

#define AX_VOICE(x)      x
#define AX_CHANNEL_LEFT  0
#define AX_CHANNEL_RIGHT 1
#define AX_BUS_MASTER    0

#define AX_NUM_CHANNELS 6

#define WIIU_MAX_VALID_CHANNELS 2

#define AX_VOICE_MONO   1
#define AX_VOICE_STEREO 2

// clang-format off
static AXVoiceDeviceMixData DEVICE_MIX[0x06];
// clang-format on
