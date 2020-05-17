#pragma once

#include <citro2d.h>
#include <3ds.h>

#define appletMainLoop aptMainLoop

#define romfsMountFromFsdev(path, offset, name)

#define MAX_GAMEPADS 1

#define GetMasterVolume

void userAppInit();
void userAppExit();

typedef C3D_RenderTarget Renderer;
typedef C2D_Sprite TextureHandle;

typedef struct _Frame Frame;

typedef C2D_Font FontHandle;
typedef C2D_TextBuf TextBuffer;
typedef C2D_Text TextHandle;
