#include "driver/display/AttributeLayout.hpp"
#include "common/Exception.hpp"

#include <coreinit/memdefaultheap.h>
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

        std::copy_n(&stream, 1, &this->streams[index]);
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
                active.push_back(this->streams[i].getStream());
        }

        const auto size = GX2CalcFetchShaderSizeEx(active.size(), GX2_FETCH_SHADER_TESSELLATION_NONE,
                                                   GX2_TESSELLATION_MODE_DISCRETE);

        this->program = memalign(GX2_SHADER_PROGRAM_ALIGNMENT, size);

        if (!this->program)
            throw love::Exception("Failed to allocate fetch shader.");

        GX2InitFetchShaderEx(&this->fetchShader, (uint8_t*)this->program, active.size(), active.data(),
                             GX2_FETCH_SHADER_TESSELLATION_NONE, GX2_TESSELLATION_MODE_DISCRETE);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, this->program, size);
    }
} // namespace love
