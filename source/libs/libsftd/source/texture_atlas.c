#include <stdlib.h>
#include <string.h>
#include "texture_atlas.h"

texture_atlas *texture_atlas_create(int width, int height, sf2d_texfmt format, sf2d_place place)
{
	texture_atlas *atlas = malloc(sizeof(*atlas));
	if (!atlas)
		return NULL;

	bp2d_rectangle rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = width;
	rect.h = height;

	atlas->tex = sf2d_create_texture(width, height, format, place);
	sf2d_texture_set_params(atlas->tex, GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR));
	sf2d_texture_tile32(atlas->tex);

	atlas->bp_root = bp2d_create(&rect);
	atlas->htab = int_htab_create(256);

	return atlas;
}

void texture_atlas_free(texture_atlas *atlas)
{
	sf2d_free_texture(atlas->tex);
	bp2d_free(atlas->bp_root);
	int_htab_free(atlas->htab);
	free(atlas);
}

int texture_atlas_insert(texture_atlas *atlas, unsigned int character, const void *image, int width, int height, int bitmap_left, int bitmap_top, int advance_x, int advance_y, int glyph_size)
{
	bp2d_size size;
	size.w = width;
	size.h = height;

	bp2d_position pos;
	if (bp2d_insert(atlas->bp_root, &size, &pos) == 0)
		return 0;

	atlas_htab_entry *entry = malloc(sizeof(*entry));

	entry->rect.x = pos.x;
	entry->rect.y = pos.y;
	entry->rect.w = width;
	entry->rect.h = height;
	entry->bitmap_left = bitmap_left;
	entry->bitmap_top = bitmap_top;
	entry->advance_x = advance_x;
	entry->advance_y = advance_y;
	entry->glyph_size = glyph_size;

	int_htab_insert(atlas->htab, character, entry);

	int i, j;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			sf2d_set_pixel(atlas->tex, pos.x + j, pos.y + i,
				__builtin_bswap32(*(unsigned int *)(image + (j + i*width)*4)));
		}
	}

	GSPGPU_FlushDataCache(atlas->tex->data, atlas->tex->data_size);

	return 1;
}

int texture_atlas_exists(texture_atlas *atlas, unsigned int character)
{
	return int_htab_find(atlas->htab, character) != NULL;
}

void texture_atlas_get(texture_atlas *atlas, unsigned int character, bp2d_rectangle *rect, int *bitmap_left, int *bitmap_top, int *advance_x, int *advance_y, int *glyph_size)
{
	atlas_htab_entry *entry = int_htab_find(atlas->htab, character);

	rect->x = entry->rect.x;
	rect->y = entry->rect.y;
	rect->w = entry->rect.w;
	rect->h = entry->rect.h;
	*bitmap_left = entry->bitmap_left;
	*bitmap_top = entry->bitmap_top;
	*advance_x = entry->advance_x;
	*advance_y = entry->advance_y;
	*glyph_size = entry->glyph_size;
}
