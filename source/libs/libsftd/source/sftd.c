#include "sftd.h"
#include <wchar.h>
#include <sf2d.h>
#include <ft2build.h>
#include FT_CACHE_H
#include FT_FREETYPE_H

static int sftd_initialized = 0;
static FT_Library ftlibrary;
static FTC_Manager ftcmanager;

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
	FTC_CMapCache cmapcache;
	FTC_ImageCache imagecache;
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

	error = FTC_Manager_New(
		ftlibrary,
		0,  /* use default */
		0,  /* use default */
		0,  /* use default */
		&ftc_face_requester,  /* use our requester */
		NULL,                 /* user data  */
		&ftcmanager);

	if (error != FT_Err_Ok) {
		FT_Done_FreeType(ftlibrary);
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

	FTC_Manager_Done(ftcmanager);

	sftd_initialized = 0;
	return 1;
}

sftd_font *sftd_load_font_file(const char *filename)
{
	sftd_font *font = malloc(sizeof(*font));

	size_t len = strlen(filename);

	font->filename = malloc(len + 1);
	strcpy(font->filename, filename);
	font->filename[len] = '\0';

	FTC_CMapCache_New(ftcmanager, &font->cmapcache);
	FTC_ImageCache_New(ftcmanager, &font->imagecache);

	font->from = SFTD_LOAD_FROM_FILE;

	return font;
}

sftd_font *sftd_load_font_mem(const void *buffer, unsigned int size)
{
	sftd_font *font = malloc(sizeof(*font));
	font->font_buffer = buffer;
	font->buffer_size = size;

	FTC_CMapCache_New(ftcmanager, &font->cmapcache);
	FTC_ImageCache_New(ftcmanager, &font->imagecache);

	font->from = SFTD_LOAD_FROM_MEM;

	return font;
}

void sftd_free_font(sftd_font *font)
{
	if (font) {
		FTC_FaceID face_id = (FTC_FaceID)font;
		FTC_Manager_RemoveFaceID(ftcmanager, face_id);
		if (font->from == SFTD_LOAD_FROM_FILE) {
			free(font->filename);
		}
		free(font);
	}
}

static void draw_bitmap(FT_Bitmap *bitmap, int x, int y, unsigned int color)
{
	//This is too ugly
	sf2d_texture *tex = sf2d_create_texture(bitmap->width, bitmap->rows, GPU_RGBA8, SF2D_PLACE_TEMP);

	int j, k;
	for (j = 0; j < bitmap->rows; j++) {
		for (k = 0; k < bitmap->width; k++) {
			((u32 *)tex->data)[j*tex->pow2_w + k] = __builtin_bswap32((color & ~0xFF) | bitmap->buffer[j*bitmap->width + k]);
		}
	}

	sf2d_texture_tile32(tex);
	sf2d_draw_texture(tex, x, y);
	sf2d_free_texture(tex);
}

void sftd_draw_text(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const char *text)
{
	FTC_FaceID face_id = (FTC_FaceID)font;
	FT_Face face;
	FTC_Manager_LookupFace(ftcmanager, face_id, &face);

	FT_Int charmap_index;
	charmap_index = FT_Get_Charmap_Index(face->charmap);

	FT_Glyph glyph;
	FT_Bool use_kerning = FT_HAS_KERNING(face);
	FT_UInt glyph_index, previous = 0;
	int pen_x = x;
	int pen_y = y;

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

		FTC_ImageCache_LookupScaler(font->imagecache, &scaler, flags, glyph_index, &glyph, NULL);
		if (glyph->format == FT_GLYPH_FORMAT_BITMAP) {
			FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

			draw_bitmap(&bitmap_glyph->bitmap, pen_x + bitmap_glyph->left + x, pen_y - bitmap_glyph->top + y, color);

			pen_x += bitmap_glyph->root.advance.x >> 16;
			pen_y += bitmap_glyph->root.advance.y >> 16;
		}

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
	FTC_Manager_LookupFace(ftcmanager, face_id, &face);

	FT_Int charmap_index;
	charmap_index = FT_Get_Charmap_Index(face->charmap);

	FT_Glyph glyph;
	FT_Bool use_kerning = FT_HAS_KERNING(face);
	FT_UInt glyph_index, previous = 0;
	int pen_x = x;
	int pen_y = y;

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

		FTC_ImageCache_LookupScaler(font->imagecache, &scaler, flags, glyph_index, &glyph, NULL);
		if (glyph->format == FT_GLYPH_FORMAT_BITMAP) {
			FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

			draw_bitmap(&bitmap_glyph->bitmap, pen_x + bitmap_glyph->left + x, pen_y - bitmap_glyph->top + y, color);

			pen_x += bitmap_glyph->root.advance.x >> 16;
			pen_y += bitmap_glyph->root.advance.y >> 16;
		}

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

