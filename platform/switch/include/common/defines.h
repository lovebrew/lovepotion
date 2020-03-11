#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <switch.h>

#include "common/mmath.h"

using love::Rect;

#define MAX_GAMEPADS 4

#define SetMasterVolume

static inline Result AudioInit()
{
    const AudioRendererConfig arConfig =
    {
        .output_rate     = AudioRendererOutputRate_48kHz,
        .num_voices      = 24,
        .num_effects     = 0,
        .num_sinks       = 1,
        .num_mix_objs    = 1,
        .num_mix_buffers = 2,
    };

    return audrenInitialize(&arConfig);
}

static inline Result AudioExit()
{
    audrenExit();
}

typedef SDL_Renderer Renderer;

typedef SDL_Window Frame;

typedef JoystickPosition StickPosition;

typedef HidVibrationValue VibrationValue;

typedef SDL_Texture * Texture;

typedef TTF_Font * FontHandle;

typedef struct _TickCounter {
} TickCounter;

typedef struct _TextBuffer {
} TextBuffer;

typedef struct _TextHandle {
} TextHandle;

typedef struct
{
    float r;
    float g;
    float b;
    float a;
} Color;
