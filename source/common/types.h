#pragma once

typedef enum 
{
	TYPE_UNKNOWN = -1,
	TYPE_OGG = 0
} love_source_type;

typedef struct 
{ 
	float x, y, z;
} vertex;

typedef struct
{
	vertex position;
	u32 color;
} vertexPositions;

extern char BUTTONS[32][32];