#include <string.h>
#include <math.h>
#include "sf2d_private.h"

//stolen from staplebutt
void GPU_SetDummyTexEnv(u8 num)
{
	GPU_SetTexEnv(num,
		GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0),
		GPU_TEVSOURCES(GPU_PREVIOUS, 0, 0),
		GPU_TEVOPERANDS(0,0,0),
		GPU_TEVOPERANDS(0,0,0),
		GPU_REPLACE,
		GPU_REPLACE,
		0xFFFFFFFF);
}

void vector_mult_matrix4x4(const float *msrc, const sf2d_vector_3f *vsrc, sf2d_vector_3f *vdst)
{
	vdst->x = msrc[0*4 + 0]*vsrc->x + msrc[0*4 + 1]*vsrc->y + msrc[0*4 + 2]*vsrc->z + msrc[0*4 + 3];
	vdst->y = msrc[1*4 + 0]*vsrc->x + msrc[1*4 + 1]*vsrc->y + msrc[1*4 + 2]*vsrc->z + msrc[1*4 + 3];
	vdst->z = msrc[2*4 + 0]*vsrc->x + msrc[2*4 + 1]*vsrc->y + msrc[2*4 + 2]*vsrc->z + msrc[2*4 + 3];
}

void matrix_gpu_set_uniform(const float *m, u32 startreg)
{
	float mu[4*4];

	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			mu[i*4 + j] = m[i*4 + (3-j)];
		}
	}

	GPU_SetFloatUniform(GPU_VERTEX_SHADER, startreg, (u32 *)mu, 4);
}

void matrix_copy(float *dst, const float *src)
{
	memcpy(dst, src, sizeof(float)*4*4);
}

void matrix_identity4x4(float *m)
{
	m[0] = m[5] = m[10] = m[15] = 1.0f;
	m[1] = m[2] = m[3] = 0.0f;
	m[4] = m[6] = m[7] = 0.0f;
	m[8] = m[9] = m[11] = 0.0f;
	m[12] = m[13] = m[14] = 0.0f;
}

void matrix_mult4x4(const float *src1, const float *src2, float *dst)
{
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			dst[i*4 + j] = 0.0f;
			for (k = 0; k < 4; k++) {
				dst[i*4 + j] += src1[i*4 + k]*src2[k*4 + j];
			}
		}
	}
}

void matrix_set_z_rotation(float *m, float rad)
{
	float c = cosf(rad);
	float s = sinf(rad);

	matrix_identity4x4(m);

	m[0] = c;
	m[1] = -s;
	m[4] = s;
	m[5] = c;
}

void matrix_rotate_z(float *m, float rad)
{
	float mr[4*4], mt[4*4];
	matrix_set_z_rotation(mr, rad);
	matrix_mult4x4(mr, m, mt);
	matrix_copy(m, mt);
}

void matrix_set_scaling(float *m, float x_scale, float y_scale, float z_scale)
{
	matrix_identity4x4(m);
	m[0] = x_scale;
	m[5] = y_scale;
	m[10] = z_scale;
}

void matrix_swap_xy(float *m)
{
	float ms[4*4], mt[4*4];
	matrix_identity4x4(ms);

	ms[0] = 0.0f;
	ms[1] = 1.0f;
	ms[4] = 1.0f;
	ms[5] = 0.0f;

	matrix_mult4x4(ms, m, mt);
	matrix_copy(m, mt);
}

void matrix_init_orthographic(float *m, float left, float right, float bottom, float top, float near, float far)
{
	float mo[4*4], mp[4*4];
	
	mo[0x0] = 2.0f/(right-left);
	mo[0x1] = 0.0f;
	mo[0x2] = 0.0f;
	mo[0x3] = -(right+left)/(right-left);

	mo[0x4] = 0.0f;
	mo[0x5] = 2.0f/(top-bottom);
	mo[0x6] = 0.0f;
	mo[0x7] = -(top+bottom)/(top-bottom);

	mo[0x8] = 0.0f;
	mo[0x9] = 0.0f;
	mo[0xA] = -2.0f/(far-near);
	mo[0xB] = (far+near)/(far-near);

	mo[0xC] = 0.0f;
	mo[0xD] = 0.0f;
	mo[0xE] = 0.0f;
	mo[0xF] = 1.0f;
	
	matrix_identity4x4(mp);
	mp[0xA] = 0.5;
	mp[0xB] = -0.5;

	//Convert Z [-1, 1] to [-1, 0] (PICA shiz)
	matrix_mult4x4(mp, mo, m);
	// Rotate 180 degrees
	matrix_rotate_z(m, M_PI);
	// Swap X and Y axis
	matrix_swap_xy(m);
}

//Grabbed from: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
unsigned int next_pow2(unsigned int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v+1;
}
