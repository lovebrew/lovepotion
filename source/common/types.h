#pragma once

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
	vertex2 quad;
} texturePositions;

typedef struct
{
	texturePositions * position;
} vertexArray;

typedef struct
{
	int r;
	int g;
	int b;
	int a;
} pixel;

extern char BUTTONS[32][32];