#include "common/runtime.h"
#include "imagedata.h"
#include "filesystem.h"
#include "image.h"
#include "crendertarget.h"
#include "graphics.h"

using love::Image;

#define TEXTURE_TRANSFER_FLAGS (GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) |  GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
		GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) |  GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

void Image::BasicInit(bool VRAM)
{
	this->texture = new C3D_Tex();
	this->width = 0;
	this->height = 0;

	this->isPremultiplied = false;

	this->VRAM = VRAM;
}

void Image::BasicInit(int width, int height)
{
	this->texture = new C3D_Tex();
	this->width = width;
	this->height = height;

	C3D_TexInit(this->texture, this->NextPow2(width), this->NextPow2(height), GPU_RGBA8);

	C3D_TexSetFilter(this->texture, magFilter, minFilter);
	
	C3D_TexSetWrap(this->texture, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}

const char * Image::Init(const char * path, bool memory, bool VRAM)
{
	this->path = path;
	this->BasicInit(VRAM);

	if (path != nullptr)
	{
		if (!love::Filesystem::Instance()->Exists(path))
			return path;

		if (!memory)
			this->DecodeFile();
	}
	
	return nullptr;
}

void Image::DecodeMemory(const unsigned char * in, size_t size)
{
	unsigned char * buffer = nullptr;
	unsigned textureWidth, textureHeight;

	lodepng::State state;
	lodepng_decode32(&buffer, &textureWidth, &textureHeight, in, size);

	if (state.info_png.color.colortype != 6)
		this->isPremultiplied = true;

	this->Decode(buffer, textureWidth, textureHeight);
}

void Image::DecodeFile()
{
	unsigned char * buffer = nullptr;
	unsigned textureWidth, textureHeight;
	unsigned error;

	lodepng::State state;

	error = lodepng_decode32_file(&buffer, &textureWidth, &textureHeight, this->path);

	if (error)
	{
		printf("Error %u: %s\nFilename: %s\n", error, lodepng_error_text(error), this->path);
		return;
	}

	if (state.info_png.color.colortype != 6)
		this->isPremultiplied = true;

	this->Decode(buffer, textureWidth, textureHeight);
}

void Image::Decode(unsigned char * buffer, unsigned textureWidth, unsigned textureHeight)
{
	this->width = textureWidth;
	this->height = textureHeight;

	int citroWidth = this->NextPow2(textureWidth);
	int citroHeight = this->NextPow2(textureHeight);

	if (!this->VRAM)
		C3D_TexInit(this->texture, citroWidth, citroHeight, GPU_RGBA8);
	else
		C3D_TexInitVRAM(this->texture, citroWidth, citroHeight, GPU_RGBA8);

	u8 * gpuBuffer = (u8 *)linearAlloc(citroWidth * citroHeight * 4);

	if (isPremultiplied)
	{
		u32 * src = (u32 *)buffer;
		u32 * dst = (u32 *)gpuBuffer;

		for (unsigned y = 0; y < textureHeight; y++)
		{
			dst = (u32*)gpuBuffer + citroWidth * y;
			for (unsigned x = 0; x < textureWidth; x++)
			{
				u32 clr = *(src++);
				*(dst++) = __builtin_bswap32(clr);
			}
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
				u8 r = *(src++);
				u8 g = *(src++);
				u8 b = *(src++);
				u8 a = *(src++);

				float aa = (1.0f / 255.0f) * a;

				*(dst++) = a;
				*(dst++) = b*aa;
				*(dst++) = g*aa;
				*(dst++) = r*aa;
			}
			dst += (citroWidth - textureWidth) * 4;
		}

	}
	

	this->LoadTexture(gpuBuffer, citroWidth, citroHeight);

	free(buffer);
	linearFree(gpuBuffer);
}

void Image::LoadTexture(void * data, int citroWidth, int citroHeight)
{
	GSPGPU_FlushDataCache((u32 *)data, citroWidth * citroHeight * 4);

	if (!love::Graphics::Instance()->InFrame())
	{	
		C3D_SafeDisplayTransfer((u32 *)data, GX_BUFFER_DIM(citroWidth, citroHeight), (u32 *)this->texture->data, GX_BUFFER_DIM(this->texture->width, this->texture->height), TEXTURE_TRANSFER_FLAGS);

		gspWaitForPPF();
	}
	else
	{	
		C3D_FrameSplit(0);

		GX_DisplayTransfer((u32 *)data, GX_BUFFER_DIM(citroWidth, citroHeight), (u32 *)this->texture->data, GX_BUFFER_DIM(this->texture->width, this->texture->height), TEXTURE_TRANSFER_FLAGS);
	}

	C3D_TexSetFilter(this->texture, magFilter, minFilter);

	C3D_TexSetWrap(this->texture, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}

void Image::SetFilter(const char * min, const char * mag)
{
	if (strncmp(min, "nearest", 7) == 0)
		this->minFilter = GPU_NEAREST;
	else if (strncmp(min, "linear", 6) == 0)
		this->minFilter = GPU_LINEAR;

	if (strncmp(mag, "nearest", 7) == 0)
		this->magFilter = GPU_NEAREST;
	else if (strncmp(mag, "linear", 6) == 0)
		this->magFilter = GPU_LINEAR;

	C3D_TexSetFilter(this->texture, this->magFilter, this->minFilter);
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