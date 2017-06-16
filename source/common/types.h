#pragma once

typedef enum 
{
	TYPE_UNKNOWN = -1,
	TYPE_OGG = 0
} love_source_type;

typedef struct
{
	float u, v;
} vertex2;

typedef struct 
{ 
	float x, y, z;
} vertex3;

typedef struct
{
	vertex3 position;
	u32 color;
} vertexPositions;

typedef struct
{
	vertex3 position;
	u32 color;
	vertex2 quad;
} texturePositions;

extern char BUTTONS[32][32];