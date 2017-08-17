#include "common/runtime.h"
#include "imagedata.h"

using love::ImageData;

const char * ImageData::Init(const char * path)
{
	this->path = path;

	const char * error = this->Decode();

	if (error)
		return error;

	return nullptr;
}

const char * ImageData::Decode()
{
	this->buffer = nullptr;
	unsigned textureWidth, textureHeight;

	lodepng_decode32_file(&buffer, &textureWidth, &textureHeight, this->path);

	this->width = textureWidth;
	this->height = textureHeight;
}

void ImageData::SetPixel(int x, int y, u32 color)
{
	this->buffer[x + y * this->width] = color;
}

int ImageData::GetWidth()
{
	return this->width;
}

int ImageData::GetHeight()
{
	return this->height;
}