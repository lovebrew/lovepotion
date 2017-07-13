#include "common/runtime.h"
#include "quad.h"
#include "file.h"
#include "glyph.h"
#include "image.h"
#include "font.h"
#include "wrap_graphics.h"

size_t bufferSize = 0;
void * buffer = nullptr;
size_t bufferIndent = 0;

int currentR = 255;
int currentG = 255;
int currentB = 255;
int currentA = 255;

int backgroundR = 0;
int backgroundG = 0;
int backgroundB = 0;

love::Font * currentFont = nullptr;

void resetPool()
{
	bufferIndent = 0;
}

//Creates size many vertecies (x, y), aligned to align on the pool space
void * allocateAlign(size_t size, size_t align = 1) 
{
	bufferIndent += align - (bufferIndent % align); // prealign
	
	void * bufferObject = (void *)((buffer) + bufferIndent);
	
	bufferIndent += size;

	if (bufferIndent > bufferSize) 
		return nullptr;

	return bufferObject;
}

void generateVertecies(vertexPositions * vertecies)
{
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

	C3D_AttrInfo * attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_UNSIGNED_BYTE, 4); //v1=color

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);

	BufInfo_Add(bufInfo, vertecies, sizeof(vertexPositions), 2, 0x10);
}

void bindTexture(C3D_Tex * texture) {
	C3D_TexBind(0, texture); // 0 should be correct
	
	C3D_TexEnv * env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_CONSTANT, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
	C3D_TexEnvColor(env, graphicsGetColor());
}

void generateTextureVertecies(texturePositions * vertecies)
{
	C3D_AttrInfo * attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v2=quad

	// Configure buffers
	C3D_BufInfo * bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);

	BufInfo_Add(bufInfo, vertecies, sizeof(texturePositions), 2, 0x10);
}

void graphicsSetColor(int r, int g, int b)
{
	currentR = r;
	currentG = g;
	currentB = b;
}

void graphicsSetColor(int r, int g, int b, int a)
{
	graphicsSetColor(r, g, b);

	currentA = a;
}

void graphicsSetBackgroundColor(int r, int g, int b)
{
	backgroundR = r;
	backgroundG = g;
	backgroundB = b;
}

void graphicsSetFont(love::Font * font)
{
	currentFont = font;
}

love::Font * graphicsGetFont()
{
	return currentFont;
}

u32 graphicsGetColor()
{
	return ( ( (currentA & 0xFF) << 24 ) + ( (currentB & 0xFF) << 16) + ( (currentG & 0xFF) << 8) + ( (currentR & 0xFF) << 0 ) );
}

u32 graphicsGetBackgroundColor()
{
	return ( ( (0xFF & 0xFF) >> 24 ) + ( (backgroundB & 0xFF) >> 16) + ( (backgroundG & 0xFF) >> 8) + ( (backgroundR & 0xFF) >> 0 ) );
}

void graphicsLine(float startx, float starty, float endx, float endy)
{
	vertexPositions * vertexList = (vertexPositions *)allocateAlign(4 * sizeof(vertexPositions), 8);

	if (!vertexList)
		return;

	float dx = endx - startx;
	float dy = endy - starty;

	float nx = -dy;
	float ny = dx;

	float len = sqrt(nx * nx + ny * ny);

	if (len > 0)
	{
		nx /= len;
		ny /= len;
	}

	nx *= 1.0f * 0.5f;
	ny *= 1.0f * 0.5f;

	vertexList[0].position = { startx + nx,	starty + ny, 0.5f };
	vertexList[1].position = { startx - nx,	starty - ny, 0.5f };
	vertexList[2].position = { endx + nx,	endy + ny, 	0.5f };
	vertexList[3].position = { endx - nx,	endy - ny, 	0.5f };

	for (int i = 0; i <= 3; i++)
		vertexList[i].color = graphicsGetColor();	

	generateVertecies(vertexList);

	C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
}

void graphicsRectangle(float x, float y, float width, float height)
{
	vertexPositions * vertexList = (vertexPositions *)allocateAlign(4 * sizeof(vertexPositions), 8);

	if (!vertexList)
		return;

	vertexList[0].position = { x, 			y,			0.5f};
	vertexList[1].position = { x + width,	y, 			0.5f};
	vertexList[2].position = { x		,	y + height, 0.5f};
	vertexList[3].position = { x + width,	y + height,	0.5f};

	for (int i = 0; i <= 3; i++)
		vertexList[i].color = graphicsGetColor();

	generateVertecies(vertexList);

	C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
}

void graphicsCircle(float x, float y, float radius, float segments)
{
	vertexPositions * vertexList = (vertexPositions *)allocateAlign((segments + 2) * sizeof(vertexPositions), 8);

	if (!vertexList)
		return;

	float angle = 0;
	for (int i = 0; i <= segments; i++)
	{
		vertexList[i].position = {x + cos(angle) * radius, y + sin(angle) * radius, 0.5f};
		vertexList[i].color = graphicsGetColor();

		angle = angle + 2 * M_PI / segments;
	}

	generateVertecies(vertexList);

	C3D_DrawArrays(GPU_TRIANGLE_FAN, 0, segments + 2);
}

void graphicsDraw(C3D_Tex * texture, float x, float y, int width, int height)
{	
	texturePositions * vertexList = (texturePositions *)allocateAlign(4 * sizeof(texturePositions), 8);

	if (!vertexList)
		return;

	vertexList[0].position = {x, 			y,			0.5f};
	vertexList[1].position = {x + width,	y,			0.5f};
	vertexList[2].position = {x,			y + height, 0.5f};
	vertexList[3].position = {x + width,	y + height, 0.5f};

	float u = (float)width/(float)texture->width;
	float v = (float)height/(float)texture->height;

	vertexList[0].quad = {0.0f, 0.0f};
	vertexList[1].quad = {u,	0.0f};
	vertexList[2].quad = {0.0f, v};
	vertexList[3].quad = {u,	v};

	generateTextureVertecies(vertexList);

	C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
}

void graphicsDrawQuad(C3D_Tex * texture, float x, float y, int textureX, int textureY, int textureWidth, int textureHeight)
{
	texturePositions * vertexList = (texturePositions *)allocateAlign(4 * sizeof(texturePositions), 8);

	if (!vertexList)
		return;

	vertexList[0].position = {x, 				y,					0.5f};
	vertexList[1].position = {x + textureWidth,	y,					0.5f};
	vertexList[2].position = {x,				y + textureHeight, 	0.5f};
	vertexList[3].position = {x + textureWidth,	y + textureHeight, 	0.5f};

	float u0 = textureX/(float)texture->width;
	float v0 = textureY/(float)texture->height;

	float u1 = (textureX + textureWidth)/(float)texture->width;
	float v1 = (textureY + textureHeight)/(float)texture->height;

	vertexList[0].quad = {u0,	v0};
	vertexList[1].quad = {u1,	v0};
	vertexList[2].quad = {u0,	v1};
	vertexList[3].quad = {u1,	v1};

	generateTextureVertecies(vertexList);

	C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
}

void graphicsPoints(float x, float y)
{
	graphicsCircle(x, y, 1, 100);
}

void graphicsInitWrapper()
{
	bufferSize = 0x80000;
	buffer = linearAlloc(bufferSize);
	bufferIndent = 0;
}