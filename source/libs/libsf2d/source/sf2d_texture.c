#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sf2d.h"
#include "sf2d_private.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define TEX_MIN_SIZE 8

static unsigned int nibbles_per_pixel(sf2d_texfmt format)
{
	switch (format) {
	case TEXFMT_RGBA8:
		return 8;
	case TEXFMT_RGB8:
		return 6;
	case TEXFMT_RGB5A1:
	case TEXFMT_RGB565:
	case TEXFMT_RGBA4:
	case TEXFMT_IA8:
		return 4;
	case TEXFMT_A4:
		return 1;
	case TEXFMT_I8:
	case TEXFMT_A8:
	case TEXFMT_IA4:
	default:
		return 2;
	}
}

static int calc_buffer_size(sf2d_texfmt pixel_format, int width, int height)
{
	return width * height * (nibbles_per_pixel(pixel_format)>>1);
}

sf2d_texture *sf2d_create_texture(int width, int height, sf2d_texfmt pixel_format, sf2d_place place)
{
	int pow2_w = next_pow2(width);
	int pow2_h = next_pow2(height);

	if (pow2_w < TEX_MIN_SIZE) pow2_w = TEX_MIN_SIZE;
	if (pow2_h < TEX_MIN_SIZE) pow2_h = TEX_MIN_SIZE;

	int data_size = calc_buffer_size(pixel_format, pow2_w, pow2_h);
	void *data;

	if (place == SF2D_PLACE_RAM) {
		// If there's not enough linear heap space, return
		if (linearSpaceFree() < data_size) {
			return NULL;
		}
		data = linearMemAlign(data_size, 0x80);
	} else if (place == SF2D_PLACE_VRAM) {
		// If there's not enough VRAM heap space, return
		if (vramSpaceFree() < data_size) {
			return NULL;
		}
		data = vramMemAlign(data_size, 0x80);
	} else if (place == SF2D_PLACE_TEMP) {
		if (sf2d_pool_space_free() < data_size) {
			return NULL;
		}
		data = sf2d_pool_memalign(data_size, 0x80);
	} else {
		//wot?
		return NULL;
	}

	sf2d_texture *texture = malloc(sizeof(*texture));

	texture->tiled = 0;
	texture->place = place;
	texture->pixel_format = pixel_format;
	texture->params = GPU_TEXTURE_MAG_FILTER(GPU_NEAREST)
		| GPU_TEXTURE_MIN_FILTER(GPU_NEAREST)
		| GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_BORDER)
		| GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_BORDER);
	texture->width = width;
	texture->height = height;
	texture->pow2_w = pow2_w;
	texture->pow2_h = pow2_h;
	texture->data_size = data_size;
	texture->data = data;

	if (place == SF2D_PLACE_VRAM) {
		GX_MemoryFill(texture->data, 0x00000000, (u32*)&((u8*)texture->data)[texture->data_size], GX_FILL_TRIGGER | GX_FILL_32BIT_DEPTH,
			NULL, 0x00000000, NULL, 0);
		gspWaitForPSC0();
	} else {
		memset(texture->data, 0, texture->data_size);
	}

	return texture;
}

sf2d_rendertarget *sf2d_create_rendertarget(int width, int height)
{
	sf2d_texture *tx = sf2d_create_texture(width, height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_rendertarget *rt = malloc(sizeof(*rt));
	//memcpy(rt, tx, sizeof(*tx));
	rt->texture = *tx;
	free(tx);
	//tx = * rt->texture;
	//rt->projection

	matrix_init_orthographic(rt->projection, 0.0f, width, height, 0.0f, 0.0f, 1.0f);
	matrix_rotate_z(rt->projection, M_PI / 2.0f);

	return rt;
}

void sf2d_free_texture(sf2d_texture *texture)
{
	if (texture) {
		if (texture->place == SF2D_PLACE_RAM) {
			linearFree(texture->data);
		} else if (texture->place == SF2D_PLACE_VRAM) {
			vramFree(texture->data);
		}
		free(texture);
	}
}

void sf2d_free_target(sf2d_rendertarget *target)
{
	sf2d_free_texture(&(target->texture));
	//free(target); // unnecessary since the texture is the start of the target struct
}

void sf2d_clear_target(sf2d_rendertarget *target, u32 color) {
	if (color == 0) { // if fully transparent, take a shortcut
		memset(target->texture.data, 0, target->texture.width * target->texture.height * 4);
		sf2d_texture_tile32(&(target->texture));
		return;
	}

	color = ((color>>24)&0x000000FF) | ((color>>8)&0x0000FF00) | ((color<<8)&0x00FF0000) | ((color<<24)&0xFF000000); // reverse byte order

	int itarget = target->texture.width * target->texture.height;
	for (int i = 0; i < itarget; i++) { memcpy(target->texture.data + i*4, &color, 4); }

	sf2d_texture_tile32(&(target->texture));
}

void sf2d_fill_texture_from_RGBA8(sf2d_texture *dst, const void *rgba8, int source_w, int source_h)
{
	// TODO: add support for non-RGBA8 textures

	u8 *tmp = linearAlloc(dst->pow2_w * dst->pow2_h * 4);
	int i, j;
	for (i = 0; i < source_h; i++) {
		for (j = 0; j < source_w; j++) {
			((u32 *)tmp)[i*dst->pow2_w + j] = ((u32 *)rgba8)[i*source_w + j];
		}
	}
	memcpy(dst->data, tmp, dst->pow2_w*dst->pow2_h*4);
	linearFree(tmp);

	sf2d_texture_tile32(dst);
}

sf2d_texture *sf2d_create_texture_mem_RGBA8(const void *src_buffer, int src_w, int src_h, sf2d_texfmt pixel_format, sf2d_place place)
{
	sf2d_texture *tex = sf2d_create_texture(src_w, src_h, pixel_format, place);
	if (tex == NULL) return NULL;
	sf2d_fill_texture_from_RGBA8(tex, src_buffer, src_w, src_h);
	return tex;
}

void sf2d_bind_texture(const sf2d_texture *texture, GPU_TEXUNIT unit)
{
	GPU_SetTextureEnable(unit);

	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_TEXTURE0, GPU_TEXTURE0),
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_TEXTURE0, GPU_TEXTURE0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_REPLACE, GPU_REPLACE,
		0xFFFFFFFF
	);

	GPU_SetTexture(
		unit,
		(u32 *)osConvertVirtToPhys(texture->data),
		texture->pow2_w,
		texture->pow2_h,
		texture->params,
		texture->pixel_format
	);
}

void sf2d_bind_texture_color(const sf2d_texture *texture, GPU_TEXUNIT unit, u32 color)
{
	GPU_SetTextureEnable(unit);

	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_CONSTANT, GPU_CONSTANT),
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_CONSTANT, GPU_CONSTANT),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_MODULATE, GPU_MODULATE,
		color
	);

	GPU_SetTexture(
		unit,
		(u32 *)osConvertVirtToPhys(texture->data),
		texture->pow2_w,
		texture->pow2_h,
		texture->params,
		texture->pixel_format
	);
}

void sf2d_bind_texture_parameters(const sf2d_texture *texture, GPU_TEXUNIT unit, unsigned int params)
{
	GPU_SetTextureEnable(unit);

	GPU_SetTexEnv(
		0,
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_TEXTURE0, GPU_TEXTURE0),
		GPU_TEVSOURCES(GPU_TEXTURE0, GPU_TEXTURE0, GPU_TEXTURE0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_TEVOPERANDS(0, 0, 0),
		GPU_REPLACE, GPU_REPLACE,
		0xFFFFFFFF
	);

	GPU_SetTexture(
		unit,
		(u32 *)osConvertVirtToPhys(texture->data),
		texture->pow2_w,
		texture->pow2_h,
		params,
		texture->pixel_format
	);
}

void sf2d_texture_set_params(sf2d_texture *texture, u32 params)
{
	texture->params = params;
}

int sf2d_texture_get_params(const sf2d_texture *texture)
{
	return texture->params;
}

static inline void sf2d_draw_texture_generic(const sf2d_texture *texture, int x, int y)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	int w = texture->width;
	int h = texture->height;

	vertices[0].position = (sf2d_vector_3f){(float)x,   (float)y,   SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){(float)x+w, (float)y,   SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){(float)x,   (float)y+h, SF2D_DEFAULT_DEPTH};
	vertices[3].position = (sf2d_vector_3f){(float)x+w, (float)y+h, SF2D_DEFAULT_DEPTH};

	float u = texture->width/(float)texture->pow2_w;
	float v = texture->height/(float)texture->pow2_h;

	vertices[0].texcoord = (sf2d_vector_2f){0.0f, 0.0f};
	vertices[1].texcoord = (sf2d_vector_2f){u,    0.0f};
	vertices[2].texcoord = (sf2d_vector_2f){0.0f, v};
	vertices[3].texcoord = (sf2d_vector_2f){u,    v};

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_texture(const sf2d_texture *texture, int x, int y)
{
	sf2d_bind_texture(texture, GPU_TEXUNIT0);
	sf2d_draw_texture_generic(texture, x, y);
}

void sf2d_draw_texture_blend(const sf2d_texture *texture, int x, int y, u32 color)
{
	sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
	sf2d_draw_texture_generic(texture, x, y);
}

static inline void sf2d_draw_texture_rotate_hotspot_generic(const sf2d_texture *texture, int x, int y, float rad, float center_x, float center_y)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	const float w = texture->width;
	const float h = texture->height;

	vertices[0].position.x = -center_x;
	vertices[0].position.y = -center_y;
	vertices[0].position.z = SF2D_DEFAULT_DEPTH;

	vertices[1].position.x = w - center_x;
	vertices[1].position.y = -center_y;
	vertices[1].position.z = SF2D_DEFAULT_DEPTH;

	vertices[2].position.x = -center_x;
	vertices[2].position.y = h - center_y;
	vertices[2].position.z = SF2D_DEFAULT_DEPTH;

	vertices[3].position.x = w - center_x;
	vertices[3].position.y = h - center_y;
	vertices[3].position.z = SF2D_DEFAULT_DEPTH;

	float u = w/(float)texture->pow2_w;
	float v = h/(float)texture->pow2_h;

	vertices[0].texcoord = (sf2d_vector_2f){0.0f, 0.0f};
	vertices[1].texcoord = (sf2d_vector_2f){u,    0.0f};
	vertices[2].texcoord = (sf2d_vector_2f){0.0f, v};
	vertices[3].texcoord = (sf2d_vector_2f){u,    v};

	const float c = cosf(rad);
	const float s = sinf(rad);
	int i;
	for (i = 0; i < 4; ++i) { // Rotate and translate
		float _x = vertices[i].position.x;
		float _y = vertices[i].position.y;
		vertices[i].position.x = _x*c - _y*s + x;
		vertices[i].position.y = _x*s + _y*c + y;
	}

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_texture_rotate_hotspot(const sf2d_texture *texture, int x, int y, float rad, float center_x, float center_y)
{
	sf2d_bind_texture(texture, GPU_TEXUNIT0);
	sf2d_draw_texture_rotate_hotspot_generic(texture, x, y, rad, center_x, center_y);
}

void sf2d_draw_texture_rotate_hotspot_blend(const sf2d_texture *texture, int x, int y, float rad, float center_x, float center_y, u32 color)
{
	sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
	sf2d_draw_texture_rotate_hotspot_generic(texture, x, y, rad, center_x, center_y);
}

void sf2d_draw_texture_rotate(const sf2d_texture *texture, int x, int y, float rad)
{
	sf2d_draw_texture_rotate_hotspot(texture, x, y, rad,
		texture->width/2.0f,
		texture->height/2.0f);
}

void sf2d_draw_texture_rotate_blend(const sf2d_texture *texture, int x, int y, float rad, u32 color)
{
	sf2d_draw_texture_rotate_hotspot_blend(texture, x, y, rad,
		texture->width/2.0f,
		texture->height/2.0f,
		color);
}

static inline void sf2d_draw_texture_rotate_scale_hotspot_generic(const sf2d_texture *texture, int x, int y, float rad, float scale_x, float scale_y, float center_x, float center_y)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	const float w = texture->width;
	const float h = texture->height;
    
    vertices[0].position.x = -center_x * scale_x;
	vertices[0].position.y = -center_y * scale_y;
	vertices[0].position.z = SF2D_DEFAULT_DEPTH;

	vertices[1].position.x = (w - center_x) * scale_x;
	vertices[1].position.y = -center_y * scale_y;
	vertices[1].position.z = SF2D_DEFAULT_DEPTH;

	vertices[2].position.x = -center_x * scale_x;
	vertices[2].position.y = (h - center_y) * scale_y;
	vertices[2].position.z = SF2D_DEFAULT_DEPTH;

	vertices[3].position.x = (w - center_x) * scale_x;
	vertices[3].position.y = h - center_y * scale_y;
	vertices[3].position.z = SF2D_DEFAULT_DEPTH;

	float u = w/(float)texture->pow2_w;
	float v = h/(float)texture->pow2_h;

	vertices[0].texcoord = (sf2d_vector_2f){0.0f, 0.0f};
	vertices[1].texcoord = (sf2d_vector_2f){u,    0.0f};
	vertices[2].texcoord = (sf2d_vector_2f){0.0f, v};
	vertices[3].texcoord = (sf2d_vector_2f){u,    v};

	const float c = cosf(rad);
	const float s = sinf(rad);
	int i;
	for (i = 0; i < 4; ++i) { // Rotate and translate
		float _x = vertices[i].position.x;
		float _y = vertices[i].position.y;
		vertices[i].position.x = _x*c - _y*s + x;
		vertices[i].position.y = _x*s + _y*c + y;
	}

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_texture_rotate_scale_hotspot(const sf2d_texture *texture, int x, int y, float rad, float scale_x, float scale_y, float center_x, float center_y)
{
    sf2d_bind_texture(texture, GPU_TEXUNIT0);
	sf2d_draw_texture_rotate_scale_hotspot_generic(texture, x, y, rad, scale_x, scale_y, center_x, center_y);
}

void sf2d_draw_texture_rotate_scale_hotspot_blend(const sf2d_texture *texture, int x, int y, float rad, float scale_x, float scale_y, float center_x, float center_y, u32 color)
{
    sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
	sf2d_draw_texture_rotate_scale_hotspot_generic(texture, x, y, rad, scale_x, scale_y, center_x, center_y);
}

static inline void sf2d_draw_texture_part_generic(const sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	vertices[0].position = (sf2d_vector_3f){(float)x,       (float)y,       SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){(float)x+tex_w, (float)y,       SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){(float)x,       (float)y+tex_h, SF2D_DEFAULT_DEPTH};
	vertices[3].position = (sf2d_vector_3f){(float)x+tex_w, (float)y+tex_h, SF2D_DEFAULT_DEPTH};

	float u0 = tex_x/(float)texture->pow2_w;
	float v0 = tex_y/(float)texture->pow2_h;
	float u1 = (tex_x+tex_w)/(float)texture->pow2_w;
	float v1 = (tex_y+tex_h)/(float)texture->pow2_h;

	vertices[0].texcoord = (sf2d_vector_2f){u0, v0};
	vertices[1].texcoord = (sf2d_vector_2f){u1, v0};
	vertices[2].texcoord = (sf2d_vector_2f){u0, v1};
	vertices[3].texcoord = (sf2d_vector_2f){u1, v1};

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_texture_part(const sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h)
{
	sf2d_bind_texture(texture, GPU_TEXUNIT0);
	sf2d_draw_texture_part_generic(texture, x, y, tex_x, tex_y, tex_w, tex_h);
}

void sf2d_draw_texture_part_blend(const sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, u32 color)
{
	sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
	sf2d_draw_texture_part_generic(texture, x, y, tex_x, tex_y, tex_w, tex_h);
}

static inline void sf2d_draw_texture_scale_generic(const sf2d_texture *texture, int x, int y, float x_scale, float y_scale)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	int ws = texture->width * x_scale;
	int hs = texture->height * y_scale;

	vertices[0].position = (sf2d_vector_3f){(float)x,    (float)y,    SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){(float)x+ws, (float)y,    SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){(float)x,    (float)y+hs, SF2D_DEFAULT_DEPTH};
	vertices[3].position = (sf2d_vector_3f){(float)x+ws, (float)y+hs, SF2D_DEFAULT_DEPTH};

	float u = texture->width/(float)texture->pow2_w;
	float v = texture->height/(float)texture->pow2_h;

	vertices[0].texcoord = (sf2d_vector_2f){0.0f, 0.0f};
	vertices[1].texcoord = (sf2d_vector_2f){u,    0.0f};
	vertices[2].texcoord = (sf2d_vector_2f){0.0f, v};
	vertices[3].texcoord = (sf2d_vector_2f){u,    v};

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_texture_scale(const sf2d_texture *texture, int x, int y, float x_scale, float y_scale)
{
	sf2d_bind_texture(texture, GPU_TEXUNIT0);
	sf2d_draw_texture_scale_generic(texture, x, y, x_scale, y_scale);
}

void sf2d_draw_texture_scale_blend(const sf2d_texture *texture, int x, int y, float x_scale, float y_scale, u32 color)
{
	sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
	sf2d_draw_texture_scale_generic(texture, x, y, x_scale, y_scale);
}

static inline void sf2d_draw_texture_part_scale_generic(const sf2d_texture *texture, float x, float y, float tex_x, float tex_y, float tex_w, float tex_h, float x_scale, float y_scale)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	float u0 = tex_x/(float)texture->pow2_w;
	float v0 = tex_y/(float)texture->pow2_h;
	float u1 = (tex_x+tex_w)/(float)texture->pow2_w;
	float v1 = (tex_y+tex_h)/(float)texture->pow2_h;

	vertices[0].texcoord = (sf2d_vector_2f){u0, v0};
	vertices[1].texcoord = (sf2d_vector_2f){u1, v0};
	vertices[2].texcoord = (sf2d_vector_2f){u0, v1};
	vertices[3].texcoord = (sf2d_vector_2f){u1, v1};

	tex_w *= x_scale;
	tex_h *= y_scale;

	vertices[0].position = (sf2d_vector_3f){(float)x,       (float)y,       SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){(float)x+tex_w, (float)y,       SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){(float)x,       (float)y+tex_h, SF2D_DEFAULT_DEPTH};
	vertices[3].position = (sf2d_vector_3f){(float)x+tex_w, (float)y+tex_h, SF2D_DEFAULT_DEPTH};

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_texture_part_scale(const sf2d_texture *texture, float x, float y, float tex_x, float tex_y, float tex_w, float tex_h, float x_scale, float y_scale)
{
	sf2d_bind_texture(texture, GPU_TEXUNIT0);
	sf2d_draw_texture_part_scale_generic(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale);
}

void sf2d_draw_texture_part_scale_blend(const sf2d_texture *texture, float x, float y, float tex_x, float tex_y, float tex_w, float tex_h, float x_scale, float y_scale, u32 color)
{
	sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
	sf2d_draw_texture_part_scale_generic(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale);
}

static inline void sf2d_draw_texture_part_rotate_scale_generic(const sf2d_texture *texture, int x, int y, float rad, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	int w2 = (tex_w * x_scale)/2.0f;
	int h2 = (tex_h * y_scale)/2.0f;

	vertices[0].position = (sf2d_vector_3f){(float)-w2, (float)-h2, SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){(float) w2, (float)-h2, SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){(float)-w2, (float) h2, SF2D_DEFAULT_DEPTH};
	vertices[3].position = (sf2d_vector_3f){(float) w2, (float) h2, SF2D_DEFAULT_DEPTH};

	float u0 = tex_x/(float)texture->pow2_w;
	float v0 = tex_y/(float)texture->pow2_h;
	float u1 = (tex_x+tex_w)/(float)texture->pow2_w;
	float v1 = (tex_y+tex_h)/(float)texture->pow2_h;

	vertices[0].texcoord = (sf2d_vector_2f){u0, v0};
	vertices[1].texcoord = (sf2d_vector_2f){u1, v0};
	vertices[2].texcoord = (sf2d_vector_2f){u0, v1};
	vertices[3].texcoord = (sf2d_vector_2f){u1, v1};

	const float c = cosf(rad);
	const float s = sinf(rad);
	int i;
	for (i = 0; i < 4; ++i) { // Rotate and translate
		float _x = vertices[i].position.x;
		float _y = vertices[i].position.y;
		vertices[i].position.x = _x*c - _y*s + x;
		vertices[i].position.y = _x*s + _y*c + y;
	}

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_texture_part_rotate_scale(const sf2d_texture *texture, int x, int y, float rad, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale)
{
	sf2d_bind_texture(texture, GPU_TEXUNIT0);
	sf2d_draw_texture_part_rotate_scale_generic(texture, x, y, rad, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale);
}

void sf2d_draw_texture_part_rotate_scale_blend(const sf2d_texture *texture, int x, int y, float rad, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale, u32 color)
{
	sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
	sf2d_draw_texture_part_rotate_scale_generic(texture, x, y, rad, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale);
}

static inline void sf2d_draw_texture_depth_generic(const sf2d_texture *texture, int x, int y, signed short z)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	int w = texture->width;
	int h = texture->height;
	float depth = z/32768.0f + 0.5f;

	vertices[0].position = (sf2d_vector_3f){(float)x,   (float)y,   depth};
	vertices[1].position = (sf2d_vector_3f){(float)x+w, (float)y,   depth};
	vertices[2].position = (sf2d_vector_3f){(float)x,   (float)y+h, depth};
	vertices[3].position = (sf2d_vector_3f){(float)x+w, (float)y+h, depth};

	float u = texture->width/(float)texture->pow2_w;
	float v = texture->height/(float)texture->pow2_h;

	vertices[0].texcoord = (sf2d_vector_2f){0.0f, 0.0f};
	vertices[1].texcoord = (sf2d_vector_2f){u,    0.0f};
	vertices[2].texcoord = (sf2d_vector_2f){0.0f, v};
	vertices[3].texcoord = (sf2d_vector_2f){u,    v};

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

void sf2d_draw_texture_depth(const sf2d_texture *texture, int x, int y, signed short z)
{
	sf2d_bind_texture(texture, GPU_TEXUNIT0);
	sf2d_draw_texture_depth_generic(texture, x, y, z);
}

void sf2d_draw_texture_depth_blend(const sf2d_texture *texture, int x, int y, signed short z, u32 color)
{
	sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
	sf2d_draw_texture_depth_generic(texture, x, y, z);
}


void sf2d_draw_quad_uv(const sf2d_texture *texture, float left, float top, float right, float bottom, float u0, float v0, float u1, float v1, unsigned int params)
{
	sf2d_vertex_pos_tex *vertices = sf2d_pool_memalign(4 * sizeof(sf2d_vertex_pos_tex), 8);
	if (!vertices) return;

	vertices[0].position = (sf2d_vector_3f){left,  top,    SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){right, top,    SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){left,  bottom, SF2D_DEFAULT_DEPTH};
	vertices[3].position = (sf2d_vector_3f){right, bottom, SF2D_DEFAULT_DEPTH};

	vertices[0].texcoord = (sf2d_vector_2f){u0, v0};
	vertices[1].texcoord = (sf2d_vector_2f){u1, v0};
	vertices[2].texcoord = (sf2d_vector_2f){u0, v1};
	vertices[3].texcoord = (sf2d_vector_2f){u1, v1};

	sf2d_bind_texture_parameters(texture, GPU_TEXUNIT0, params);

	GPU_SetAttributeBuffers(
		2, // number of attributes
		(u32*)osConvertVirtToPhys(vertices),
		GPU_ATTRIBFMT(0, 3, GPU_FLOAT) | GPU_ATTRIBFMT(1, 2, GPU_FLOAT),
		0xFFFC, //0b1100
		0x10,
		1, //number of buffers
		(u32[]){0x0}, // buffer offsets (placeholders)
		(u64[]){0x10}, // attribute permutations for each buffer
		(u8[]){2} // number of attributes for each buffer
	);

	GPU_DrawArray(GPU_TRIANGLE_STRIP, 0, 4);
}

// Grabbed from Citra Emulator (citra/src/video_core/utils.h)
static inline u32 morton_interleave(u32 x, u32 y)
{
	u32 i = (x & 7) | ((y & 7) << 8); // ---- -210
	i = (i ^ (i << 2)) & 0x1313;      // ---2 --10
	i = (i ^ (i << 1)) & 0x1515;      // ---2 -1-0
	i = (i | (i >> 7)) & 0x3F;
	return i;
}

//Grabbed from Citra Emulator (citra/src/video_core/utils.h)
static inline u32 get_morton_offset(u32 x, u32 y, u32 bytes_per_pixel)
{
    u32 i = morton_interleave(x, y);
    unsigned int offset = (x & ~7) * 8;
    return (i + offset) * bytes_per_pixel;
}

void sf2d_set_pixel(sf2d_texture *texture, int x, int y, u32 new_color)
{
	y = (texture->pow2_h - 1 - y);
	if (texture->tiled) {
		u32 coarse_y = y & ~7;
		u32 offset = get_morton_offset(x, y, 4) + coarse_y * texture->pow2_w * 4;
		*(u32 *)(texture->data + offset) = new_color;
	} else {
		((u32 *)texture->data)[x + y * texture->pow2_w] = new_color;
	}
}

u32 sf2d_get_pixel(sf2d_texture *texture, int x, int y)
{
	y = (texture->pow2_h - 1 - y);
	if (texture->tiled) {
		u32 coarse_y = y & ~7;
		u32 offset = get_morton_offset(x, y, 4) + coarse_y * texture->pow2_w * 4;
		return *(u32 *)(texture->data + offset);
	} else {
		return ((u32 *)texture->data)[x + y * texture->pow2_w];
	}
}


void sf2d_texture_tile32(sf2d_texture *texture)
{
	if (texture->tiled) return;

	// TODO: add support for non-RGBA8 textures
	u8 *tmp = linearAlloc(texture->pow2_w * texture->pow2_h * 4);

	int i, j;
	for (j = 0; j < texture->pow2_h; j++) {
		for (i = 0; i < texture->pow2_w; i++) {

			u32 coarse_y = j & ~7;
			u32 dst_offset = get_morton_offset(i, j, 4) + coarse_y * texture->pow2_w * 4;

			u32 v = ((u32 *)texture->data)[i + (texture->pow2_h - 1 - j)*texture->pow2_w];
			*(u32 *)(tmp + dst_offset) = __builtin_bswap32(v); /* RGBA8 -> ABGR8 */
		}
	}

	memcpy(texture->data, tmp, texture->pow2_w*texture->pow2_h*4);
	linearFree(tmp);

	texture->tiled = 1;
}
