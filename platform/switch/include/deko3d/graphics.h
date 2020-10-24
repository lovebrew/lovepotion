#pragma once

#include "modules/graphics/graphics.h"
#include "deko3d/deko.h"

namespace love::deko3d
{
    class Graphics : public love::Graphics
    {
        void Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth) override;

        void Present() override;
    };
}