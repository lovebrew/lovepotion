#include "common/runtime.h"
#include "common/drawable.h"
#include "modules/window.h"

Drawable::Drawable(char * type) : Object(type) {}

u32 * Drawable::LoadPNG(const char * path, char * buffer, size_t memorySize)
{
    u32 * out = nullptr;
    FILE * input;

    if (memorySize == -1)
        input = fopen(path, "rb");
    else
        input = fmemopen(buffer, memorySize, "rb");

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

    //output ABGR
    png_set_bgr(png);
    png_set_swap_alpha(png);

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

    if ((out = new (std::nothrow) u32[this->width * this->height]) == nullptr)
    {
        for (int j = 0; j < height; j++)
            free(row_pointers[j]);

        delete[] row_pointers;
        return nullptr;
    }

    for(int j = 0; j < height; j++)
    {
        png_bytep row = row_pointers[j];
        for(int i = 0; i < this->width; i++)
        {
            png_bytep px = &(row[i * 4]);
            memcpy(&out[j * this->width + i], px, sizeof(u32));
        }
        free(row_pointers[j]); // free the completed row, to avoid having to iterate over the whole thing again
    }
    delete[] row_pointers;

    return out;
}

void Drawable::LoadImage(u32 * buffer)
{
    C3D_Tex * texture = new (std::nothrow) C3D_Tex;
    if (!texture)
    {
        delete[] buffer;
        return;
    }
    this->image.tex = texture;

    u32 w_pow2 = NextPO2((u32)this->width);
    u32 h_pow2 = NextPO2((u32)this->height);

    this->subTexture.width = this->width;
    this->subTexture.height = this->height;
    this->subTexture.left = 0.0f;
    this->subTexture.top = 1.0f;
    this->subTexture.right = this->width/(float)w_pow2;
    this->subTexture.bottom = 1.0-(this->height/(float)h_pow2);

    this->image.subtex = &this->subTexture;

    C3D_TexInit(this->image.tex, w_pow2, h_pow2, GPU_RGBA8);

    memset(this->image.tex->data, 0, this->image.tex->size);

    for(int j = 0; j < this->height; j++)
    {
        for(int i = 0; i < this->width; i++)
        {
            u32 dst = ((((j >> 3) * (w_pow2 >> 3) + (i >> 3)) << 6) + ((i & 1) | ((j & 1) << 1) | ((i & 2) << 1) | ((j & 2) << 2) | ((i & 4) << 2) | ((j & 4) << 3))) * 4;
            memcpy(((u8 *)(this->image.tex->data)) + dst, &buffer[j * width + i], sizeof(u32));
        }
    }

    delete[] buffer;

    C3D_TexSetFilter(this->image.tex, GPU_LINEAR, GPU_LINEAR);
    this->image.tex->border = 0x00FFFFFF;
    C3D_TexSetWrap(this->image.tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}

void Drawable::Draw(float x, float y, float rotation, float scalarX, float scalarY, Color color)
{
    if (rotation == 0)
        C2D_DrawImageAt(this->image, x, y, 0.5, nullptr, scalarX, scalarY);
    else
        C2D_DrawImageAtRotated(this->image, x, y, 0.5f, rotation, nullptr, scalarX, scalarY);
}

int Drawable::GetWidth()
{
    return this->width;
}

int Drawable::GetHeight()
{
    return this->height;
}

Tex3DS_SubTexture Drawable::GetSubTexture()
{
    return this->subTexture;
}

void Drawable::SetSubTexture(const Tex3DS_SubTexture & subTexture)
{
    this->image.subtex = &subTexture;
}
