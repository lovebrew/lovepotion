#include "common/runtime.h"
#include "image.h"

using love::Image;

char * Image::Init(const char * path)
{

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