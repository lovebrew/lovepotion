#include "common/runtime.h"

#include "objects/file/file.h"
#include "objects/image/image.h"

Image::Image(const char * path)
{
	File exists(path);

	exists.Open("rb");
	if (!exists.IsOpen())
		throw Exception("File does not exit: %s", path);
	exists.Close();

	unsigned width, height;
	//u16 error = lodepng::decode(sheet, width, height, path);

	//if (error)
	//	throw Exception("Error %u: %s\nFilename: %s\n", error, lodepng_error_text(error), path);

	this->width = width;
	this->height = height;

}

u16 Image::GetWidth()
{
	return this->width;
}

u16 Image::GetHeight()
{
	return this->height;
}

vector<u8> Image::GetImage()
{
	return this->sheet;
}

Image::~Image() {}
