#pragma once

#include "modules/font/fontmodulec.h"
#include "objects/bcfntrasterizer/bcfntrasterizer.h"

namespace love
{
    class FontModule : public common::FontModule
    {
        virtual FontModule() {};

        Rasterizer* NewRasterizer(FileData* data) override;

        Rasterizer* NewBCFNTRasterizer(Data* data, int size);

        Rasterizer* NewBCFNTRasterizer(int size);
    };
} // namespace love
