#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <switch.h>

#include "common/mmath.h"

using love::Rect;

#define MAX_GAMEPADS 4

#define FlushAudioCache armDCacheFlush

typedef AudioDriverWaveBuf waveBuffer;

typedef SDL_Renderer Renderer;

typedef SDL_Window Frame;

typedef JoystickPosition StickPosition;

typedef HidVibrationValue VibrationValue;

typedef SDL_Texture * TextureHandle;

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
