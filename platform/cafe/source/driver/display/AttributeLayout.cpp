#include "driver/display/AttributeLayout.hpp"
#include "common/Exception.hpp"
#include "driver/display/Attribute.hpp"

#include <algorithm>
#include <coreinit/memdefaultheap.h>
#include <gx2/mem.h>

#include <gx2/shaders.h>
#include <gx2/state.h>
#include <malloc.h>
#include <vector>

namespace love
{
    GX2AttributeLayout::GX2AttributeLayout() : fetchShader {}, program(nullptr), dirty(true)
    {}

    GX2AttributeLayout::~GX2AttributeLayout()
    {
        this->destroy();
    }

    void GX2AttributeLayout::reset()
    {
        this->destroy();
        this->dirty = true;
    }

    void GX2AttributeLayout::destroy()
    {
        if (this->program)
        {
            std::free(this->program);
            this->program = nullptr;
        }

        this->fetchShader.program = nullptr;
    }

    void GX2AttributeLayout::bind()
    {
        GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);
        GX2SetFetchShader(&this->fetchShader);
    }

    void GX2AttributeLayout::rebuild(const std::vector<GX2AttribStream>& state)
    {
        const auto count = state.size();

        const auto type = GX2_FETCH_SHADER_TESSELLATION_NONE;
        const auto mode = GX2_TESSELLATION_MODE_DISCRETE;
        const auto size = GX2CalcFetchShaderSizeEx(count, type, mode);

        this->program = (uint8_t*)memalign(GX2_SHADER_PROGRAM_ALIGNMENT, size);

        if (!this->program)
            throw love::Exception("Failed to allocate fetch shader.");

        GX2InitFetchShaderEx(&this->fetchShader, this->program, count, state.data(), type, mode);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, this->program, size);
        this->dirty = false;
    }
} // namespace love
