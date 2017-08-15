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

int currentR = 0xFF;
int currentG = 0xFF;
int currentB = 0xFF;
int currentA = 0xFF;

int backgroundR = 0x00;
int backgroundG = 0x00;
int backgroundB = 0x00;

float currentDepth = 0;

float translateX = 0;
float translateY = 0;

GPU_TEXTURE_FILTER_PARAM minFilter;
GPU_TEXTURE_FILTER_PARAM magFilter;

bool graphicsPushed = false;

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

void bindTexture(C3D_Tex * texture) 
{
	C3D_TexBind(0, texture); // 0 should be correct
	
	C3D_TexEnv * env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_CONSTANT, 0);

	C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA);
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

void graphicsSet3D(bool enable)
{
	gfxSet3D(enable);
}

void graphicsSetDepth(float depth)
{
	currentDepth = depth;
}

void translateCoords(float * x, float * y)
{
	if (graphicsPushed)
	{
		*x += translateX;
		*y += translateY;
	}

	if (!gfxIs3D() || (gfxIs3D() && currentScreen != GFX_TOP))
		return;

	float slider = CONFIG_3D_SLIDERSTATE;
	if (currentSide == GFX_LEFT)
		*x -= (slider * currentDepth);
	else if (currentSide == GFX_RIGHT)
		*x += (slider * currentDepth);
}

int graphicsGetColor(int color)
{
	int returnColor;
	switch (color)
	{
		case 0:
			returnColor = currentR;
			break;
		case 1:
			returnColor = currentG;
			break;
		case 2:
			returnColor = currentB;
			break;
		case 3:
			returnColor = currentA;
			break;
	}

	return returnColor;
}

void graphicsSetColor(int r, int g, int b, int a)
{
	currentR = r * float(a) / 255.0f;
	currentG = g * float(a) / 255.0f;
	currentB = b * float(a) / 255.0f;
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
	return ( ( (currentA & 0xFF) << 24 ) | ( (currentB & 0xFF) << 16) | ( (currentG & 0xFF) << 8) | ( (currentR & 0xFF) << 0 ) );
}

u32 graphicsGetBackgroundColor()
{
	return ( ( (backgroundR & 0xFF) << 24 ) + ( (backgroundG & 0xFF) << 16) + ( (backgroundB & 0xFF) << 8) + ( (0xFF & 0xFF) << 0 ) );
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

	translateCoords(&x, &y);

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
	
	translateCoords(&x, &y);

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

void graphicsDraw(C3D_Tex * texture, float x, float y, int width, int height, float rotation, float scalarX, float scalarY)
{
	texturePositions * vertexList = (texturePositions *)allocateAlign(4 * sizeof(texturePositions), 8);

	if (!vertexList)
		return;

	float scaleWidth = width * scalarX;
	float scaleHeight = height * scalarY;

	translateCoords(&x, &y);

	vertexList[0].position = {x, 				y,				 0.5f};
	vertexList[1].position = {x + scaleWidth,	y,				 0.5f};
	vertexList[2].position = {x,				y + scaleHeight, 0.5f};
	vertexList[3].position = {x + scaleWidth,	y + scaleHeight, 0.5f};

	float u = (float)width/(float)texture->width;
	float v = (float)height/(float)texture->height;

	vertexList[0].quad = {0.0f, 0.0f};
	vertexList[1].quad = {u,	0.0f};
	vertexList[2].quad = {0.0f, v};
	vertexList[3].quad = {u,	v};

	generateTextureVertecies(vertexList);

	C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
}

void graphicsDrawQuad(C3D_Tex * texture, float x, float y, int textureX, int textureY, int textureWidth, int textureHeight, float rotation, float scalarX, float scalarY)
{
	texturePositions * vertexList = (texturePositions *)allocateAlign(4 * sizeof(texturePositions), 8);

	if (!vertexList)
		return;
	
	float scaleWidth = textureWidth * scalarX;
	float scaleHeight = textureHeight * scalarY;

	translateCoords(&x, &y);

	vertexList[0].position = {x, 				y,					0.5f};
	vertexList[1].position = {x + scaleWidth,	y,					0.5f};
	vertexList[2].position = {x,				y + scaleHeight, 	0.5f};
	vertexList[3].position = {x + scaleWidth,	y + scaleHeight, 	0.5f};

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

void graphicsPush()
{
	if (currentScreen == renderScreen)
		graphicsPushed = true;
}

void graphicsTranslate(float x, float y)
{
	if (currentScreen == renderScreen)
	{
		translateX += x;
		translateY += y;
	}
}

void graphicsPop()
{
	if (currentScreen == renderScreen)
	{
		graphicsPushed = false;
		translateX = 0;
		translateY = 0;
	}
}

void graphicsPrint(const char * text, float x, float y)
{
	graphicsPrintf(text, x, y, NULL);
}

void graphicsSetScissor(bool disable, float x, float y, float width, float height)
{
	if (width < 0 || height < 0)
		console->ThrowError("Scissor cannot have negative width or height.");

	GPU_SCISSORMODE mode = GPU_SCISSOR_NORMAL;
	if (disable)
		mode = GPU_SCISSOR_DISABLE;

	C3D_SetScissor(mode, x, y, width, height);
}

void graphicsSetDefaultFilter(const char * min, const char * mag)
{
	if (strncmp(min, "nearest", 7) == 0)
		minFilter = GPU_NEAREST;
	else if (strncmp(min, "linear", 6) == 0)
		minFilter = GPU_LINEAR;

	if (strncmp(mag, "nearest", 7) == 0)
		magFilter = GPU_NEAREST;
	else if (strncmp(mag, "linear", 6) == 0)
		magFilter = GPU_LINEAR;
}

void graphicsPrintf(const char * text, float x, float y, float limit)
{
	float originX = x;
	float originY = y;

	love::Font * currFont = graphicsGetFont();

	if (currFont == nullptr)
		return;

	C3D_Tex * texture = currFont->GetSheet()->GetTexture();

	int width, wrapWidth = 0;
	bindTexture(texture);

	for (int i = 0; i < strlen(text); i++)
	{
		love::Glyph * glyph = currFont->GetGlyph(text[i]);

		if (text[i] == '\n' || (limit != NULL && wrapWidth >= limit))
		{
			x = originX;
			y = y + currFont->GetHeight() + 2;
			width = 0;
			wrapWidth = 0;
		}
		else
		{
			if (glyph == nullptr)
				return;
		
			love::Quad * quad = glyph->GetQuad();

			if (i > 0)
			{	
				if (currFont->GetGlyph(text[i - 1]) != nullptr)
					width += currFont->GetGlyph(text[i - 1])->GetXAdvance(); //+ glyph->GetXOffset();
			}
			wrapWidth += currFont->GetWidth(text[i]);

			graphicsDrawQuad(texture, x + width + glyph->GetXOffset(), y + glyph->GetYOffset(), quad->GetX(), quad->GetY(), quad->GetWidth(), quad->GetHeight(), 0, 1, 1);
		}
	}
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