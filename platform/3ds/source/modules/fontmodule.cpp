#include "modules/font/fontmodule.h"

using namespace love;

/* -- BCFNT Rasterizer -- */

/* love.font.newRasterizer */
Rasterizer* FontModule::NewRasterizer(FileData* data)
{
    if (BCFNTRasterizer::Accepts(data))
        return NewBCFNTRasterizer(data, 12);

    throw love::Exception("Invalid font file: %s", data->GetFilename().c_str());
}

/* If accepted, create the new BCFNTRasterizer */
Rasterizer* FontModule::NewBCFNTRasterizer(Data* data, int size)
{
    return new BCFNTRasterizer(data, size);
}

Rasterizer* FontModule::NewBCFNTRasterizer(int size)
{
    return new BCFNTRasterizer(Font::SystemFontType::TYPE_STANDARD, size);
}
