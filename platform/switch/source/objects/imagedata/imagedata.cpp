#include "common/runtime.h"

#include "common/drawable.h"
#include "modules/window.h"
#include "modules/display.h"
#include "objects/imagedata/imagedata.h"

ImageData::ImageData(int width, int height) : Object("ImageData")
{
    this->width = width;
    this->height = height;
}

ImageData::ImageData(const char * filename)
{
    this->buffer = this->LoadFilename(filename);
}

u32 * ImageData::LoadBuffer()
{
    u32 * out = nullptr;

    return out;
}

u32 * ImageData::LoadFilename(const char * filename)
{
    u32 * out = nullptr;
    FILE * input;

    if (filename != NULL)
        input = fopen(filename, "rb");

    if (!input)
        return nullptr;

    constexpr size_t sigLength = 8;
    u8 sig[sigLength] = {0};

    size_t size = fread(sig, sizeof(u8), sigLength, input);

    fseek(input, 0, SEEK_SET);

    if (size < sigLength || png_sig_cmp(sig, 0, sigLength))
    {
        fclose(input);
        return nullptr;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    png_infop info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(input);
        return nullptr;
    }

    png_init_io(png, input);
    png_read_info(png, info);

    this->width = png_get_image_width(png, info);
    this->height = png_get_image_height(png, info);

    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth  = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, ABGR format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt
    if(bit_depth == 16)
        png_set_strip_16(png);

    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS) != 0)
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if(color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    png_bytep * row_pointers = new (std::nothrow) png_bytep[this->height];
    if (row_pointers == nullptr)
    {
        png_destroy_read_struct(&png, &info, NULL);
        return nullptr;
    }

    for (int y = 0; y < this->height; y++)
        row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png, info));

    png_read_image(png, row_pointers);

    fclose(input);
    png_destroy_read_struct(&png, &info, NULL);

    //out = (u32 *)linearAlloc(this->width * this->height);
    out = new (std::nothrow) u32[this->width * this->height];

    if (out == NULL)
    {
        for (int j = 0; j < this->height; j++)
            free(row_pointers[j]);

        delete[] row_pointers;
        return nullptr;
    }

    for(int j = this->height - 1; j >= 0; j--)
    {
        png_bytep row = row_pointers[j];
        for(int i = 0; i < this->width; i++)
        {
            png_bytep px = &(row[i * 4]);
            memcpy(&out[(((this->height - 1) - j) * this->width) + i], px, sizeof(u32));
        }
        free(row_pointers[j]); // free the completed row, to avoid having to iterate over the whole thing again
    }
    delete[] row_pointers;

    return out;
}

u32 * ImageData::GetBuffer()
{
    return this->buffer;
}

int ImageData::GetSize()
{
    return this->size;
}

SDL_Color ImageData::GetPixel(int x, int y)
{
    SDL_Color color = {0, 0, 0, 0};

    u32 pixel = this->buffer[y * this->width + x];

    color.r = pixel & 0x000000FF;
    color.g = pixel & 0x0000FF00 >> 8;
    color.b = pixel & 0x00FF0000 >> 16;
    color.a = pixel & 0xFF000000 >> 24;

    return color;
}
