#include "common/runtime.h"
#include "modules/font/fontmodule.h"

#include "common/data.h"

using namespace love;

love::Rasterizer * FontModule::NewRasterizer(love::FileData * data)
{
    if (love::TrueTypeRasterizer::Accepts(data))
        return this->NewTrueTypeRasterizer(data, 12, love::TrueTypeRasterizer::HINTING_NORMAL);

    throw love::Exception("Invalid font file: %s", data->GetFilename().c_str());
}

love::Rasterizer * FontModule::NewTrueTypeRasterizer(love::Data * data, int size, love::TrueTypeRasterizer::Hinting hinting)
{
    return new love::TrueTypeRasterizer(data, size, hinting);
}

love::Rasterizer * FontModule::NewTrueTypeRasterizer(void * data, size_t dataSize, int size, love::TrueTypeRasterizer::Hinting hinting)
{
    return new love::TrueTypeRasterizer(data, dataSize, size, hinting);
}