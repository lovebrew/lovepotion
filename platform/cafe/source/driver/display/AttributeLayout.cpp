#include "driver/display/AttributeLayout.hpp"

#include <gx2/mem.h>

#include <bit>
#include <malloc.h>
#include <memory>
#include <vector>

#include "common/debug.hpp"

namespace love
{
    GX2AttributeLayout::GX2AttributeLayout() : fetchShader {}, program(nullptr), dirty(true)
    {
        this->streams.fill(GX2Attribute {});
    }

    GX2AttributeLayout::~GX2AttributeLayout()
    {
        this->destroy();
    }

    void GX2AttributeLayout::reset()
    {
        this->destroy();
        this->streams.fill({});
        this->dirty = true;
    }

    void GX2AttributeLayout::set(uint32_t index, const GX2Attribute& stream)
    {
        if (stream != this->streams[index])
            this->dirty = true;

        this->streams[index] = stream;
    }

    void GX2AttributeLayout::bind()
    {
        if (this->dirty)
            this->rebuild();

        GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);
        GX2SetFetchShader(&this->fetchShader);
    }

    void GX2AttributeLayout::destroy()
    {
        if (this->fetchShader.program)
        {
            std::free(this->fetchShader.program);
            this->fetchShader.program = nullptr;
        }
    }

    void GX2AttributeLayout::rebuild()
    {
        std::vector<GX2AttribStream> active {};
        for (uint32_t i = 0; i < this->streams.size(); ++i)
        {
            if (this->streams[i].isEnabled())
                active.push_back(this->streams[i]);
        }

        const auto count = active.size();
        const auto size  = GX2CalcFetchShaderSizeEx(count, TESSELATION_NONE, TESSELATION_DISCRETE);

        this->destroy();

        this->fetchShader.size    = size;
        this->fetchShader.program = memalign(GX2_SHADER_PROGRAM_ALIGNMENT, size);

        if (!this->fetchShader.program)
            throw std::bad_alloc();

        // clang-format off
        GX2InitFetchShaderEx(&this->fetchShader, (uint8_t*)this->fetchShader.program, count, active.data(), TESSELATION_NONE, TESSELATION_DISCRETE);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, this->fetchShader.program, size);
        // clang-format on

        this->dirty = false;
    }
} // namespace love
