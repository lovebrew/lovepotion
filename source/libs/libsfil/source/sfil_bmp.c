#include "sfil.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BMP_SIGNATURE (0x4D42)

typedef struct {
	unsigned short	bfType;
	unsigned int	bfSize;
	unsigned short	bfReserved1;
	unsigned short	bfReserved2;
	unsigned int	bfOffBits;
}__attribute__((packed)) BITMAPFILEHEADER;


typedef struct {
	unsigned int	biSize;
	unsigned int	biWidth;
	unsigned int	biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned int	biCompression;
	unsigned int	biSizeImage;
	unsigned int	biXPelsPerMeter;
	unsigned int	biYPelsPerMeter;
	unsigned int	biClrUsed;
	unsigned int	biClrImportant;
}__attribute__((packed)) BITMAPINFOHEADER;


static sf2d_texture *_sfil_load_BMP_generic(
	BITMAPFILEHEADER *bmp_fh,
	BITMAPINFOHEADER *bmp_ih,
	void *user_data,
	void (*seek_fn)(void *user_data, unsigned int offset),
	void (*read_fn)(void *user_data, void *buffer, unsigned int length),
	sf2d_place place)
{
	unsigned int row_size = bmp_ih->biWidth * (bmp_ih->biBitCount/8);
	if (row_size%4 != 0) {
		row_size += 4-(row_size%4);
	}

	sf2d_texture *texture = sf2d_create_texture(bmp_ih->biWidth, bmp_ih->biHeight,
		GPU_RGBA8, place);

	if (!texture)
		return NULL;

	seek_fn(user_data, bmp_fh->bfOffBits);

	int stride = texture->pow2_w * 4;

	void *buffer = malloc(row_size);
	if (!buffer) {
		sf2d_free_texture(texture);
		return NULL;
	}

	unsigned int *tex_ptr;
	unsigned int color;
	int i, x, y;

	for (i = 0; i < bmp_ih->biHeight; i++) {

		read_fn(user_data, buffer, row_size);

		y = bmp_ih->biHeight - 1 - i;
		tex_ptr = (unsigned int *)(texture->data + y*stride);

		for (x = 0; x < bmp_ih->biWidth; x++) {

			if (bmp_ih->biBitCount == 32) {		//ABGR8888
				color = *(unsigned int *)(buffer + x*4);
				*tex_ptr = (color&0xFF)<<24 | ((color>>8)&0xFF)<<16 |
					((color>>16)&0xFF)<<8 | (color>>24);

			} else if (bmp_ih->biBitCount == 24) {	//BGR888
				unsigned char *address = buffer + x*3;
				*tex_ptr = (*address)<<16 | (*(address+1))<<8 |
					(*(address+2)) | (0xFF<<24);

			} else if (bmp_ih->biBitCount == 16) {	//BGR565
				color = *(unsigned short *)(buffer + x*2);
				unsigned char r = (color       & 0x1F)  *((float)255/31);
				unsigned char g = ((color>>5)  & 0x3F)  *((float)255/63);
				unsigned char b = ((color>>11) & 0x1F)  *((float)255/31);
				*tex_ptr = ((r<<16) | (g<<8) | b | (0xFF<<24));
			}

			tex_ptr++;
		}
	}

	free(buffer);

	sf2d_texture_tile32(texture);
	return texture;
}

static void _sfil_read_bmp_file_seek_fn(void *user_data, unsigned int offset)
{
	fseek((FILE *)user_data, offset, SEEK_SET);
}

static void _sfil_read_bmp_file_read_fn(void *user_data, void *buffer, unsigned int length)
{
	fread(buffer, 1, length, (FILE *)user_data);
}

static void _sfil_read_bmp_buffer_seek_fn(void *user_data, unsigned int offset)
{
	*(unsigned int *)user_data += offset;
}

static void _sfil_read_bmp_buffer_read_fn(void *user_data, void *buffer, unsigned int length)
{
	memcpy(buffer, (void *)*(unsigned int *)user_data, length);
	*(unsigned int *)user_data += length;
}

sf2d_texture *sfil_load_BMP_file(const char *filename, sf2d_place place)
{
	FILE *fp;
	if ((fp = fopen(filename, "rb")) == NULL) {
		goto exit_error;
	}

	BITMAPFILEHEADER bmp_fh;
	fread((void *)&bmp_fh, 1, sizeof(BITMAPFILEHEADER), fp);
	if (bmp_fh.bfType != BMP_SIGNATURE) {
		goto exit_close;
	}

	BITMAPINFOHEADER bmp_ih;
	fread((void *)&bmp_ih, 1, sizeof(BITMAPINFOHEADER), fp);

	sf2d_texture *texture = _sfil_load_BMP_generic(&bmp_fh,
		&bmp_ih,
		(void *)&fp,
		_sfil_read_bmp_file_seek_fn,
		_sfil_read_bmp_file_read_fn,
		place);

	fclose(fp);
	return texture;

exit_close:
	fclose(fp);
exit_error:
	return NULL;
}


sf2d_texture *sfil_load_BMP_buffer(const void *buffer, sf2d_place place)
{
	BITMAPFILEHEADER bmp_fh;
	memcpy(&bmp_fh, buffer, sizeof(BITMAPFILEHEADER));
	if (bmp_fh.bfType != BMP_SIGNATURE) {
		goto exit_error;
	}

	BITMAPINFOHEADER bmp_ih;
	memcpy(&bmp_ih, buffer + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

	unsigned int buffer_address = (unsigned int)buffer;

	sf2d_texture *texture = _sfil_load_BMP_generic(&bmp_fh,
		&bmp_ih,
		(void *)&buffer_address,
		_sfil_read_bmp_buffer_seek_fn,
		_sfil_read_bmp_buffer_read_fn,
		place);

	return texture;
exit_error:
	return NULL;
}
