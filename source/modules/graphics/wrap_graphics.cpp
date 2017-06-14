#include "common/runtime.h"
#include "wrap_graphics.h"

size_t bufferSize = 0;
void * buffer = nullptr;
size_t bufferIndent = 0;

int currentR = 255;
int currentG = 255;
int currentB = 255;
int currentA = 255;

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
	
	// Configure the first fragment shading substage to just pass through the vertex color
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

	// Configure attributes for use with the vertex shader
	C3D_AttrInfo * attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_UNSIGNED_BYTE, 4); //v1=color

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);

	BufInfo_Add(bufInfo, vertecies, sizeof(vertexPositions), 2, 0x10);
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

u32 graphicsGetColor()
{
	return ( ( (currentA & 0xFF) << 24 ) + ( (currentB & 0xFF) << 16) + ( (currentG & 0xFF) << 8) + ( (currentR & 0xFF) << 0 ) );
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

void graphicsPoints(float x, float y)
{
	vertexPositions * vertexList = (vertexPositions *)allocateAlign(2 * sizeof(vertexPositions), 4);

	if (!vertexList)
		return;

	for (int i = 0; i <= 2; i++)
	{
		vertexList[i].position = {x, y, 0.5f};
		vertexList[i].color = graphicsGetColor();
	}

	generateVertecies(vertexList);

	C3D_DrawArrays(GPU_TRIANGLE_FAN, 0, 2);
}

void graphicsInitWrapper()
{
	bufferSize = 0x80000;
	buffer = linearAlloc(bufferSize);
	bufferIndent = 0;
}