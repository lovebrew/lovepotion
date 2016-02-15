#include "sfil.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jpeglib.h>

static sf2d_texture *_sfil_load_JPEG_generic(struct jpeg_decompress_struct *jinfo, struct jpeg_error_mgr *jerr, sf2d_place place)
{
	jpeg_start_decompress(jinfo);

	int row_bytes;
	switch (jinfo->out_color_space) {
	case JCS_RGB:
		row_bytes = jinfo->image_width * 3;
		break;
	default:
		goto exit_error;
	}

	sf2d_texture *texture = sf2d_create_texture(jinfo->image_width,
		jinfo->image_height, GPU_RGBA8, place);

	if (!texture)
		goto exit_error;

	JSAMPARRAY buffer = (JSAMPARRAY)malloc(sizeof(JSAMPROW));
	buffer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * row_bytes);

	unsigned int i, color, *tex_ptr;
	unsigned char *jpeg_ptr;
	void *row_ptr = texture->data;

	int stride = texture->pow2_w * 4;

	while (jinfo->output_scanline < jinfo->output_height) {
		jpeg_read_scanlines(jinfo, buffer, 1);
		tex_ptr = (row_ptr += stride);
		for (i = 0, jpeg_ptr = buffer[0]; i < jinfo->output_width; i++) {
			color = *(jpeg_ptr++);
			color |= *(jpeg_ptr++)<<8;
			color |= *(jpeg_ptr++)<<16;
			*(tex_ptr++) = color | 0xFF000000;
		}
	}

	jpeg_finish_decompress(jinfo);

	free(buffer[0]);
	free(buffer);

	sf2d_texture_tile32(texture);
	return texture;

exit_error:
	jpeg_abort_decompress(jinfo);
	return NULL;
}


sf2d_texture *sfil_load_JPEG_file(const char *filename, sf2d_place place)
{
	FILE *fp;
	if ((fp = fopen(filename, "rb")) < 0) {
		return NULL;
	}

	struct jpeg_decompress_struct jinfo;
	struct jpeg_error_mgr jerr;

	jinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&jinfo);
	jpeg_stdio_src(&jinfo, fp);
	jpeg_read_header(&jinfo, 1);

	sf2d_texture *texture = _sfil_load_JPEG_generic(&jinfo, &jerr, place);

	jpeg_destroy_decompress(&jinfo);

	fclose(fp);
	return texture;
}


sf2d_texture *sfil_load_JPEG_buffer(const void *buffer, unsigned long buffer_size, sf2d_place place)
{
	struct jpeg_decompress_struct jinfo;
	struct jpeg_error_mgr jerr;

	jinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&jinfo);
	jpeg_mem_src(&jinfo, (void *)buffer, buffer_size);
	jpeg_read_header(&jinfo, 1);

	sf2d_texture *texture = _sfil_load_JPEG_generic(&jinfo, &jerr, place);

	jpeg_destroy_decompress(&jinfo);

	return texture;
}
