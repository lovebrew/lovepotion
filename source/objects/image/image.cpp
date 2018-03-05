#include "common/runtime.h"

#include "objects/file/file.h"
#include "objects/image/image.h"

Image::Image(const char * path)
{
	string realPath = path;

	size_t found = realPath.find(".png");

	if (found != -1)
		realPath = realPath.replace(found, 4, ".bin");

	File binary(realPath.c_str());
	binary.Open("rb");

	if (!binary.IsOpen())
		Console::ThrowError("File does not exit: %s", path);

	this->sheet = binary.ReadBinary();
	binary.Close();

	this->width = 1280;
	this->height = 720;
}

int Image::GetWidth()
{
	return this->width;
}

int Image::GetHeight()
{
	return this->height;
}

u8 * Image::GetImage()
{
	return this->sheet;
}

Image::~Image()
{
	free((char *)this->sheet);
}