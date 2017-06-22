#include "common/runtime.h"
#include "image.h"

using love::Image;

#define TEXTURE_TRANSFER_FLAGS (GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) |  GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
		GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) |  GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

char * Image::Init(const char * path)
{
	this->path = path;
	this->texture = new C3D_Tex();

	this->width = 0;
	this->height = 0;

	this->Decode();

	return nullptr;
}

void Image::Decode()
{
	unsigned char * buffer;
	unsigned textureWidth, textureHeight;

	bool isPremultiplied = false;
	lodepng::State state;

	lodepng_decode32_file(&buffer, &textureWidth, &textureHeight, this->path);

	if (state.info_png.color.colortype != 6) 
		isPremultiplied = true;

	this->width = textureWidth;
	this->height = textureHeight;

	int width = this->NextPow2(textureWidth);
	int height = this->NextPow2(textureHeight);

	u8 * gpuBuffer = (u8 *)linearAlloc(width * height * 4);
	
	printf("%s: %d\n", this->path, isPremultiplied);
	if (isPremultiplied)
	{
		u32 * src = (u32 *)buffer;
		u32 * dst = (u32 *)gpuBuffer;

		for (unsigned y = 0; y < textureHeight; y++)
		{
			for (unsigned x = 0; x < textureWidth; x++)
			{
				u32 clr = *src;
				*dst = __builtin_bswap32(clr);

				src += 4;
				dst += 4;
			}
			dst += (width - textureWidth) * 4;
		}
	}
	else
	{
		u8 * src = (u8 *)buffer;
		u8 * dst = (u8 *)gpuBuffer;

		for (unsigned y = 0; y < textureHeight; y++)
		{
			for (unsigned x = 0; x < textureWidth; x++)
			{
				u8 r = *src++;
				u8 g = *src++;
				u8 b = *src++;
				u8 a = *src++;
					
				float aa = (1.0f / 255.0f) * a;
					
				*dst++ = a;
				*dst++ = b*aa;
				*dst++ = g*aa;
				*dst++ = r*aa;
			}
			dst += (width - textureWidth) * 4;
		}
	}

	this->LoadTexture(gpuBuffer, width, height);

	free(buffer);
	linearFree(gpuBuffer);
}

void Image::LoadTexture(void * data, int width, int height)
{
	GSPGPU_FlushDataCache((u32 *)data, width * height * 4);

	C3D_TexInit(this->texture, width, height, GPU_RGBA8);

	C3D_SafeDisplayTransfer((u32 *)data, GX_BUFFER_DIM(width, height), (u32 *)this->texture->data, GX_BUFFER_DIM(this->texture->width, this->texture->height), TEXTURE_TRANSFER_FLAGS);

	gspWaitForPPF();

	C3D_TexSetFilter(this->texture, GPU_LINEAR, GPU_LINEAR);

	C3D_TexSetWrap(this->texture, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}

const char * Image::GetPath()
{
	return this->path;
}

C3D_Tex * Image::GetTexture()
{
	return this->texture;
}

int Image::GetWidth()
{
	return this->width;
}

int Image::GetHeight()
{
	return this->height;
}

int Image::NextPow2(unsigned int x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return std::min(std::max(++x, 64U), 1024U); // clamp size to keep gpu from locking
}