#ifndef SF2D_PRIVATE_H
#define SF2D_PRIVATE_H

#include <3ds.h>
#include "sf2d.h"


void GPU_SetDummyTexEnv(u8 num);

// Vector operations

void vector_mult_matrix4x4(const float *msrc, const sf2d_vector_3f *vsrc, sf2d_vector_3f *vdst);

// Matrix operations

void matrix_copy(float *dst, const float *src);
void matrix_identity4x4(float *m);
void matrix_mult4x4(const float *src1, const float *src2, float *dst);
void matrix_set_z_rotation(float *m, float rad);
void matrix_rotate_z(float *m, float rad);
void matrix_set_scaling(float *m, float x_scale, float y_scale, float z_scale);
void matrix_swap_xy(float *m);

void matrix_init_orthographic(float *m, float left, float right, float bottom, float top, float near, float far);
void matrix_gpu_set_uniform(const float *m, u32 startreg);

unsigned int next_pow2(unsigned int v);

#endif
