#include "sftd.h"
#include "texture_atlas.h"
#include "bin_packing_2d.h"
#include <wchar.h>
#include <sf2d.h>
#include <ft2build.h>
#include <string.h>
#include FT_CACHE_H
#include FT_FREETYPE_H

#define ATLAS_DEFAULT_W 512
#define ATLAS_DEFAULT_H 512

static int sftd_initialized = 0;
static FT_Library ftlibrary;

typedef enum {
	SFTD_LOAD_FROM_FILE,
	SFTD_LOAD_FROM_MEM
} sftd_font_load_from;

struct sftd_font {
	sftd_font_load_from from;
	union {
		char *filename;
		struct {
			const void *font_buffer;
			unsigned int buffer_size;
		};
	};
	FTC_Manager ftcmanager;
	FTC_CMapCache cmapcache;
	FTC_ImageCache imagecache;
	texture_atlas *tex_atlas;
};

static FT_Error ftc_face_requester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *face)
{
	sftd_font *font = (sftd_font *)face_id;

	FT_Error error = FT_Err_Cannot_Open_Resource;

	if (font->from == SFTD_LOAD_FROM_FILE) {
		error = FT_New_Face(
				library,
				font->filename,
				0,
				face);
	} else if (font->from == SFTD_LOAD_FROM_MEM) {
		error = FT_New_Memory_Face(
				library,
				font->font_buffer,
				font->buffer_size,
				0,
				face);
	}
	return error;
}

int sftd_init()
{
	if (sftd_initialized) return 2;

	FT_Error error = FT_Init_FreeType(&ftlibrary);
	if (error != FT_Err_Ok) {
		return 0;
	}

	sftd_initialized = 1;
	return 1;
}

int sftd_fini()
{
	if (!sftd_initialized) return 2;

	FT_Error error = FT_Done_FreeType(ftlibrary);
	if (error != FT_Err_Ok) {
		return 0;
	}

	sftd_initialized = 0;
	return 1;
}

sftd_font *sftd_load_font_file(const char *filename)
{
	FT_Error error;

	sftd_font *font = malloc(sizeof(*font));
	if (!font)
		return NULL;

	size_t len = strlen(filename);

	font->filename = malloc(len + 1);
	strcpy(font->filename, filename);
	font->filename[len] = '\0';

	error = FTC_Manager_New(
		ftlibrary,
		0,  /* use default */
		0,  /* use default */
		0,  /* use default */
		&ftc_face_requester,  /* use our requester */
		NULL,                 /* user data  */
		&font->ftcmanager);

	if (error != FT_Err_Ok) {
		free(font->filename);
		free(font);
		return NULL;
	}

	FTC_CMapCache_New(font->ftcmanager, &font->cmapcache);
	FTC_ImageCache_New(font->ftcmanager, &font->imagecache);

	font->from = SFTD_LOAD_FROM_FILE;
	font->tex_atlas = texture_atlas_create(ATLAS_DEFAULT_W, ATLAS_DEFAULT_H,
		TEXFMT_RGBA8, SF2D_PLACE_RAM);

	return font;
}

sftd_font *sftd_load_font_mem(const void *buffer, unsigned int size)
{
	FT_Error error;

	sftd_font *font = malloc(sizeof(*font));
	if (!font)
		return NULL;

	font->font_buffer = buffer;
	font->buffer_size = size;

	error = FTC_Manager_New(
		ftlibrary,
		0,  /* use default */
		0,  /* use default */
		0,  /* use default */
		&ftc_face_requester,  /* use our requester */
		NULL,                 /* user data  */
		&font->ftcmanager);

	if (error != FT_Err_Ok) {
		free(font);
		return NULL;
	}

	FTC_CMapCache_New(font->ftcmanager, &font->cmapcache);
	FTC_ImageCache_New(font->ftcmanager, &font->imagecache);

	font->from = SFTD_LOAD_FROM_MEM;
	font->tex_atlas = texture_atlas_create(ATLAS_DEFAULT_W, ATLAS_DEFAULT_H,
		TEXFMT_RGBA8, SF2D_PLACE_RAM);

	return font;
}

void sftd_free_font(sftd_font *font)
{
	if (font) {
		FTC_FaceID face_id = (FTC_FaceID)font;
		FTC_Manager_RemoveFaceID(font->ftcmanager, face_id);
		FTC_Manager_Done(font->ftcmanager);
		if (font->from == SFTD_LOAD_FROM_FILE) {
			free(font->filename);
		}
		texture_atlas_free(font->tex_atlas);
		free(font);
	}
}

static int atlas_add_glyph(texture_atlas *atlas, unsigned int glyph_index, const FT_BitmapGlyph bitmap_glyph, int glyph_size)
{
	const FT_Bitmap *bitmap = &bitmap_glyph->bitmap;

	unsigned int *buffer = malloc(bitmap->width * bitmap->rows * 4);
	unsigned int w = bitmap->width;
	unsigned int h = bitmap->rows;

	int j, k;
	for (j = 0; j < h; j++) {
		for (k = 0; k < w; k++) {
			if (bitmap->pixel_mode == FT_PIXEL_MODE_MONO) {
				buffer[j*w + k] =
					(bitmap->buffer[j*bitmap->pitch + k/8] & (1 << (7 - k%8)))
					? 0xFF : 0;
			} else {
				buffer[j*w + k] = 0x00FFFFFF | (bitmap->buffer[j*bitmap->pitch + k] << 24);
			}
		}
	}

	int ret = texture_atlas_insert(atlas, glyph_index, buffer,
		bitmap->width, bitmap->rows,
		bitmap_glyph->left, bitmap_glyph->top,
		bitmap_glyph->root.advance.x, bitmap_glyph->root.advance.y,
		glyph_size);

	free(buffer);

	return ret;
}

void sftd_draw_text(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const char *text)
{
	FTC_FaceID face_id = (FTC_FaceID)font;
	FT_Face face;
	FTC_Manager_LookupFace(font->ftcmanager, face_id, &face);

	FT_Int charmap_index;
	charmap_index = FT_Get_Charmap_Index(face->charmap);

	FT_Glyph glyph;
	FT_Bool use_kerning = FT_HAS_KERNING(face);
	FT_UInt glyph_index, previous = 0;
	int pen_x = x;
	int pen_y = y + size;

	FTC_ScalerRec scaler;
	scaler.face_id = face_id;
	scaler.width = size;
	scaler.height = size;
	scaler.pixel = 1;

	FT_ULong flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;

	while (*text) {
		if(*text == '\n') {
			pen_x = x;
			pen_y += size;
			text++;
			continue;
		}

		glyph_index = FTC_CMapCache_Lookup(font->cmapcache, (FTC_FaceID)font, charmap_index, *text);

		if (use_kerning && previous && glyph_index) {
			FT_Vector delta;
			FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
			pen_x += delta.x >> 6;
		}

		if (!texture_atlas_exists(font->tex_atlas, glyph_index)) {
			FTC_ImageCache_LookupScaler(font->imagecache, &scaler, flags, glyph_index, &glyph, NULL);

			if (!atlas_add_glyph(font->tex_atlas, glyph_index, (FT_BitmapGlyph)glyph, size)) {
				continue;
			}
		}

		bp2d_rectangle rect;
		int bitmap_left, bitmap_top;
		int advance_x, advance_y;
		int glyph_size;

		texture_atlas_get(font->tex_atlas, glyph_index,
			&rect, &bitmap_left, &bitmap_top,
			&advance_x, &advance_y, &glyph_size);

		const float draw_scale = size/(float)glyph_size;

		sf2d_draw_texture_part_scale_blend(font->tex_atlas->tex,
			pen_x + bitmap_left * draw_scale,
			pen_y - bitmap_top * draw_scale,
			rect.x, rect.y, rect.w, rect.h,
			draw_scale,
			draw_scale,
			color);

		pen_x += (advance_x >> 16) * draw_scale;
		pen_y += (advance_y >> 16) * draw_scale;

		previous = glyph_index;
		text++;
	}
}

void sftd_draw_textf(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const char *text, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	sftd_draw_text(font, x, y, color, size, buffer);
	va_end(args);
}

void sftd_draw_wtext(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const wchar_t *text)
{
	FTC_FaceID face_id = (FTC_FaceID)font;
	FT_Face face;
	FTC_Manager_LookupFace(font->ftcmanager, face_id, &face);

	FT_Int charmap_index;
	charmap_index = FT_Get_Charmap_Index(face->charmap);

	FT_Glyph glyph;
	FT_Bool use_kerning = FT_HAS_KERNING(face);
	FT_UInt glyph_index, previous = 0;
	int pen_x = x;
	int pen_y = y + size;

	FTC_ScalerRec scaler;
	scaler.face_id = face_id;
	scaler.width = size;
	scaler.height = size;
	scaler.pixel = 1;

	FT_ULong flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;

	while (*text) {
		if(*text == '\n') {
			pen_x = x;
			pen_y += size;
			text++;
			continue;
		}

		glyph_index = FTC_CMapCache_Lookup(font->cmapcache, (FTC_FaceID)font, charmap_index, *text);

		if (use_kerning && previous && glyph_index) {
			FT_Vector delta;
			FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
			pen_x += delta.x >> 6;
		}

		if (!texture_atlas_exists(font->tex_atlas, glyph_index)) {
			FTC_ImageCache_LookupScaler(font->imagecache, &scaler, flags, glyph_index, &glyph, NULL);

			if (!atlas_add_glyph(font->tex_atlas, glyph_index, (FT_BitmapGlyph)glyph, size)) {
				continue;
			}
		}

		bp2d_rectangle rect;
		int bitmap_left, bitmap_top;
		int advance_x, advance_y;
		int glyph_size;

		texture_atlas_get(font->tex_atlas, glyph_index,
			&rect, &bitmap_left, &bitmap_top,
			&advance_x, &advance_y, &glyph_size);

		const float draw_scale = size/(float)glyph_size;

		sf2d_draw_texture_part_scale_blend(font->tex_atlas->tex,
			pen_x + bitmap_left * draw_scale,
			pen_y - bitmap_top * draw_scale,
			rect.x, rect.y, rect.w, rect.h,
			draw_scale,
			draw_scale,
			color);

		pen_x += (advance_x >> 16) * draw_scale;
		pen_y += (advance_y >> 16) * draw_scale;

		previous = glyph_index;
		text++;
	}
}

void sftd_draw_wtextf(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const wchar_t *text, ...)
{
	wchar_t buffer[256];
	va_list args;
	va_start(args, text);
	vswprintf(buffer, 256, text, args);
	sftd_draw_wtext(font, x, y, color, size, buffer);
	va_end(args);
}

int sftd_get_text_width(sftd_font *font, unsigned int size, const char *text)
{
	FTC_FaceID face_id = (FTC_FaceID)font;
	FT_Face face;
	FTC_Manager_LookupFace(font->ftcmanager, face_id, &face);

	FT_Int charmap_index;
	charmap_index = FT_Get_Charmap_Index(face->charmap);

	FT_Glyph glyph;
	FT_Bool use_kerning = FT_HAS_KERNING(face);
	FT_UInt glyph_index, previous = 0;
	int pen_x = 0;
	int pen_y = size;

	FTC_ScalerRec scaler;
	scaler.face_id = face_id;
	scaler.width = size;
	scaler.height = size;
	scaler.pixel = 1;

	FT_ULong flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;

	while (*text) {
		glyph_index = FTC_CMapCache_Lookup(font->cmapcache, (FTC_FaceID)font, charmap_index, *text);

		if (use_kerning && previous && glyph_index) {
			FT_Vector delta;
			FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
			pen_x += delta.x >> 6;
		}

		if (!texture_atlas_exists(font->tex_atlas, glyph_index)) {
			FTC_ImageCache_LookupScaler(font->imagecache, &scaler, flags, glyph_index, &glyph, NULL);

			if (!atlas_add_glyph(font->tex_atlas, glyph_index, (FT_BitmapGlyph)glyph, size)) {
				continue;
			}
		}

		bp2d_rectangle rect;
		int bitmap_left, bitmap_top;
		int advance_x, advance_y;
		int glyph_size;

		texture_atlas_get(font->tex_atlas, glyph_index,
			&rect, &bitmap_left, &bitmap_top,
			&advance_x, &advance_y, &glyph_size);

		const float draw_scale = size/(float)glyph_size;

		pen_x += (advance_x >> 16) * draw_scale;
		pen_y += (advance_y >> 16) * draw_scale;

		previous = glyph_index;
		text++;
	}
	return pen_x;
}

void sftd_draw_text_wrap(sftd_font *font, int x, int y, unsigned int color, unsigned int size, unsigned int lineWidth, const char *text)
{
	FTC_FaceID face_id = (FTC_FaceID)font;
	FT_Face face;
	FTC_Manager_LookupFace(font->ftcmanager, face_id, &face);

	FT_Int charmap_index;
	charmap_index = FT_Get_Charmap_Index(face->charmap);

	FT_Glyph glyph;
	FT_Bool use_kerning = FT_HAS_KERNING(face);
	FT_UInt glyph_index, previous = 0;
	int pen_x = x;
	int pen_y = y + size;

	FTC_ScalerRec scaler;
	scaler.face_id = face_id;
	scaler.width = size;
	scaler.height = size;
	scaler.pixel = 1;

	FT_ULong flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;

	bool isFirstLine = true;
	char buffer[strlen(text)];
	sprintf(buffer, text);
	char *currentWord;
	int currentWordLength;
	int currentCharIndex;

	currentWord = strtok(buffer, " ");
	while (currentWord) {
		currentWordLength = strlen(currentWord);
		if(pen_x + sftd_get_text_width(font, size, currentWord) >= lineWidth && !isFirstLine) {
			pen_x = x;
			pen_y += size;
		}
		isFirstLine = false;
		for(currentCharIndex = 0; currentCharIndex < currentWordLength + 1; currentCharIndex++) {
			if(currentCharIndex < currentWordLength) {
				glyph_index = FTC_CMapCache_Lookup(font->cmapcache, (FTC_FaceID)font, charmap_index, currentWord[currentCharIndex]);
			}
			else {
				glyph_index = FTC_CMapCache_Lookup(font->cmapcache, (FTC_FaceID)font, charmap_index, ' ');
			}

			// TODO get word size and linewrap if needed

			if (use_kerning && previous && glyph_index) {
				FT_Vector delta;
				FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
				pen_x += delta.x >> 6;
			}

			if (!texture_atlas_exists(font->tex_atlas, glyph_index)) {
				FTC_ImageCache_LookupScaler(font->imagecache, &scaler, flags, glyph_index, &glyph, NULL);

				if (!atlas_add_glyph(font->tex_atlas, glyph_index, (FT_BitmapGlyph)glyph, size)) {
					continue;
				}
			}

			bp2d_rectangle rect;
			int bitmap_left, bitmap_top;
			int advance_x, advance_y;
			int glyph_size;

			texture_atlas_get(font->tex_atlas, glyph_index,
				&rect, &bitmap_left, &bitmap_top,
				&advance_x, &advance_y, &glyph_size);

			const float draw_scale = size/(float)glyph_size;

			sf2d_draw_texture_part_scale_blend(font->tex_atlas->tex,
				pen_x + bitmap_left * draw_scale,
				pen_y - bitmap_top * draw_scale,
				rect.x, rect.y, rect.w, rect.h,
				draw_scale,
				draw_scale,
				color);

			pen_x += (advance_x >> 16) * draw_scale;
			pen_y += (advance_y >> 16) * draw_scale;


			previous = glyph_index;


		}
		currentWord = strtok(NULL, " ");
	}
}

void sftd_calc_bounding_box(int *boundingWidth, int *boundingHeight, sftd_font *font, unsigned int size, unsigned int lineWidth, const char *text)
{
	FTC_FaceID face_id = (FTC_FaceID)font;
	FT_Face face;
	FTC_Manager_LookupFace(font->ftcmanager, face_id, &face);

	FT_Int charmap_index;
	charmap_index = FT_Get_Charmap_Index(face->charmap);

	FT_Glyph glyph;
	FT_Bool use_kerning = FT_HAS_KERNING(face);
	FT_UInt glyph_index, previous = 0;
	int pen_x = 0;
	int pen_y = size;

	FTC_ScalerRec scaler;
	scaler.face_id = face_id;
	scaler.width = size;
	scaler.height = size;
	scaler.pixel = 1;

	FT_ULong flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;

	bool isFirstLine = true;
	char buffer[strlen(text)];
	sprintf(buffer, text);
	char *currentWord;
	int currentWordLength;
	int currentCharIndex;
	int greatesLineWidth = 0;

	currentWord = strtok(buffer, " ");
	while (currentWord) {
		currentWordLength = strlen(currentWord);
		if(pen_x + sftd_get_text_width(font, size, currentWord) >= lineWidth && !isFirstLine) {
			if(pen_x > greatesLineWidth) {
				greatesLineWidth = pen_x;
			}
			pen_x = 0;
			pen_y += size;
		}
		isFirstLine = false;
		for(currentCharIndex = 0; currentCharIndex < currentWordLength + 1; currentCharIndex++) {
			if(currentCharIndex < currentWordLength) {
				glyph_index = FTC_CMapCache_Lookup(font->cmapcache, (FTC_FaceID)font, charmap_index, currentWord[currentCharIndex]);
			}
			else {
				glyph_index = FTC_CMapCache_Lookup(font->cmapcache, (FTC_FaceID)font, charmap_index, ' ');
			}

			// TODO get word size and linewrap if needed

			if (use_kerning && previous && glyph_index) {
				FT_Vector delta;
				FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
				pen_x += delta.x >> 6;
			}

			if (!texture_atlas_exists(font->tex_atlas, glyph_index)) {
				FTC_ImageCache_LookupScaler(font->imagecache, &scaler, flags, glyph_index, &glyph, NULL);

				if (!atlas_add_glyph(font->tex_atlas, glyph_index, (FT_BitmapGlyph)glyph, size)) {
					continue;
				}
			}

			bp2d_rectangle rect;
			int bitmap_left, bitmap_top;
			int advance_x, advance_y;
			int glyph_size;

			texture_atlas_get(font->tex_atlas, glyph_index,
				&rect, &bitmap_left, &bitmap_top,
				&advance_x, &advance_y, &glyph_size);

			const float draw_scale = size/(float)glyph_size;

			pen_x += (advance_x >> 16) * draw_scale;
			pen_y += (advance_y >> 16) * draw_scale;


			previous = glyph_index;


		}
		currentWord = strtok(NULL, " ");
	}
	*boundingWidth = greatesLineWidth;
	*boundingHeight = pen_y;
}

void sftd_draw_textf_wrap(sftd_font *font, int x, int y, unsigned int color, unsigned int size, unsigned int lineWidth, const char *text, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	sftd_draw_text_wrap(font, x, y, color, size, lineWidth, buffer);
	va_end(args);
}
