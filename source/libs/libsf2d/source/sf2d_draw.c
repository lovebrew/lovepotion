#include "sf2d.h"
#include "sf2d_private.h"
#include <math.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

void sf2d_draw_line(float x0, float y0, float x1, float y1, float width, u32 color)
{
	sf2d_vertex_pos_col *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_col), 8);
	if (!vertices) return;

    float dx = x1 - x0;
    float dy = y1 - y0;

    float nx = -dy;
    float ny = dx;

    float len = sqrt(nx * nx + ny * ny);

    if (len > 0 ){
        nx /= len;
        ny /= len;
    }

    nx *= width*0.5f;
    ny *= width*0.5f;

    vertices[0].position = (sf2d_vector_3f){x0+nx, y0+ny, SF2D_DEFAULT_DEPTH};
    vertices[1].position = (sf2d_vector_3f){x0-nx, y0-ny, SF2D_DEFAULT_DEPTH};

    vertices[2].position = (sf2d_vector_3f){x1+nx, y1+ny, SF2D_DEFAULT_DEPTH};
    vertices[3].position = (sf2d_vector_3f){x1-nx, y1-ny, SF2D_DEFAULT_DEPTH};

	vertices[0].color = color;
	vertices[1].color = vertices[0].color;
	vertices[2].color = vertices[0].color;
	vertices[3].color = vertices[0].color;

	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_REPLACE, GPU_REPLACE,
		0xFFFFFFFF
	);

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 4, GPU_UNSIGNED_BYTE),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_rectangle(int x, int y, int w, int h, u32 color)
{
	sf2d_vertex_pos_col *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_col), 8);
	if (!vertices) return;

	vertices[0].position = (sf2d_vector_3f){(float)x,   (float)y,   SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){(float)x+w, (float)y,   SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){(float)x,   (float)y+h, SF2D_DEFAULT_DEPTH};
	vertices[3].position = (sf2d_vector_3f){(float)x+w, (float)y+h, SF2D_DEFAULT_DEPTH};

	vertices[0].color = color;
	vertices[1].color = vertices[0].color;
	vertices[2].color = vertices[0].color;
	vertices[3].color = vertices[0].color;

	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_REPLACE, GPU_REPLACE,
		0xFFFFFFFF
	);

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 4, GPU_UNSIGNED_BYTE),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_rectangle_rotate(int x, int y, int w, int h, u32 color, float rad)
{
	sf2d_vertex_pos_col *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_col), 8);
	if (!vertices) return;

	int w2 = w/2.0f;
	int h2 = h/2.0f;

	vertices[0].position = (sf2d_vector_3f){(float)-w2, (float)-h2, SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){(float) w2, (float)-h2, SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){(float)-w2, (float) h2, SF2D_DEFAULT_DEPTH};
	vertices[3].position = (sf2d_vector_3f){(float) w2, (float) h2, SF2D_DEFAULT_DEPTH};

	vertices[0].color = color;
	vertices[1].color = vertices[0].color;
	vertices[2].color = vertices[0].color;
	vertices[3].color = vertices[0].color;

	float m[4*4];
	matrix_set_z_rotation(m, rad);
	sf2d_vector_3f rot[4];

	int i;
	for (i = 0; i < 4; i++) {
		vector_mult_matrix4x4(m, &vertices[i].position, &rot[i]);
		vertices[i].position = (sf2d_vector_3f){rot[i].x + x + w2, rot[i].y + y + h2, rot[i].z};
	}

	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_REPLACE, GPU_REPLACE,
		0xFFFFFFFF
	);

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 4, GPU_UNSIGNED_BYTE),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_fill_circle(int x, int y, int radius, u32 color)
{
	static const int num_segments = 100;
	sf2d_vertex_pos_col *vertices = sf2d_pool_memalign((num_segments + 2) * sizeof(sf2d_vertex_pos_col), 8);
	if (!vertices) return;

	vertices[0].position = (sf2d_vector_3f){(float)x, (float)y, SF2D_DEFAULT_DEPTH};
	vertices[0].color = color;

	float theta = 2 * M_PI / (float)num_segments;
	float c = cosf(theta);
	float s = sinf(theta);
	float t;

	float xx = radius;
	float yy = 0;
	int i;

	for (i = 1; i <= num_segments; i++) {
		vertices[i].position = (sf2d_vector_3f){(float)(x + xx), (float)(y + yy), SF2D_DEFAULT_DEPTH};
		vertices[i].color = vertices[0].color;

		t = xx;
		xx = c * xx - s * yy;
		yy = s * t + c * yy;
	}

	vertices[num_segments + 1].position = vertices[1].position;
	vertices[num_segments + 1].color = vertices[1].color;

	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVSOURCES(GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_REPLACE, GPU_REPLACE,
		0xFFFFFFFF
	);

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 4, GPU_UNSIGNED_BYTE),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_FAN, 0, num_segments + 2);
}
