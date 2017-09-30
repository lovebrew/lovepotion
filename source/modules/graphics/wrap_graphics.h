#pragma once

#include "quad.h"
#include "file.h"
#include "glyph.h"
#include "imagedata.h"
#include "image.h"
#include "font.h"

extern int projection_desc;

void graphicsInitWrapper();

void resetPool();

void * allocateAlign(size_t size, size_t align);

void bindTexture(C3D_Tex * texture);

void generateVertecies(vertexPositions * vertecies);

void generateTextureVertecies(texturePositions * vertecies);

void graphicsLine(float startx, float starty, float endx, float endy);

void graphicsRectangle(float x, float y, float width, float height);

void graphicsPoints(float x, float y);

void graphicsCircle(float x, float y, float radius, float segments);

void graphicsDraw(C3D_Tex * texture, float x, float y, int width, int height, float rotation, float scalarX, float scalarY, float offsetX, float offsetY);

void graphicsDrawQuad(C3D_Tex * texture, float x, float y, int textureX, int textureY, int textureWidth, int textureHeight, float rotation, float scalarX, float scalarY, float offsetX, float offsetY);

void graphicsPrint(const char * text, float x, float y);

void graphicsPrintf(const char * text, float x, float y, float limit);

void graphicsLoadText(C3D_Tex * texture, float x, float y, int textureX, int textureY, int textureWidth, int textureHeight, float rotation, float scalarX, float scalarY, texturePositions * vertexList, int index);

int graphicsGetColor(int color);

void graphicsSetColor(int r, int g, int b, int a);

void graphicsSetBackgroundColor(int r, int g, int b);

void graphicsSetFont(love::Font * font);

void graphicsSet3D(bool enable);

void graphicsSetDepth(float depth);

void translateCoords(float * x, float * y);

void graphicsPush();

void graphicsTranslate(float x, float y);

void graphicsPop();

void graphicsSetScissor(bool disable, float x, float y, float width, float height);

void graphicsSetDefaultFilter(const char * min, const char * mag);

love::Font * graphicsGetFont();

u32 graphicsGetColor();

u32 graphicsGetBackgroundColor();