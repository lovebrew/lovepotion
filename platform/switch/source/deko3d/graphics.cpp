#include "common/runtime.h"
#include "deko3d/graphics.h"

using namespace love;

#include "s_fsh_dksh.h"
#include "s_vsh_dksh.h"

love::deko3d::Graphics::Graphics()
{
    /*
    ** This actually happens during love.graphics.setMode
    ** but that's useless on the console, so here
    */
    this->RestoreState(this->states.back());

    // We always need a default shader.
    for (int i = 0; i < Shader::STANDARD_MAX_ENUM; i++)
    {
        try
        {
            if (!Shader::standardShaders[i])
                Shader::standardShaders[i] = this->NewShader((void *)s_vsh_dksh, s_vsh_dksh_size, (void *)s_fsh_dksh, s_fsh_dksh_size);
        }
        catch (love::Exception &)
        {
            throw;
        }
    }

    // A shader should always be active, but the default shader shouldn't be
    // returned by getShader(), so we don't do setShader(defaultShader).
    if (!Shader::current)
        Shader::standardShaders[Shader::STANDARD_DEFAULT]->Attach();
}

void love::deko3d::Graphics::SetBlendMode(BlendMode mode, BlendAlpha alphamode)
{
    if (mode != this->states.back().blendMode || alphamode != this->states.back().blendAlphaMode)
    {} // flush stream draws

    if (alphamode != BLENDALPHA_PREMULTIPLIED)
    {
        const char * modestr = "unknown";
        switch (mode)
        {
            case BLEND_LIGHTEN:
            case BLEND_DARKEN:
            case BLEND_MULTIPLY:
                Graphics::GetConstant(mode, modestr);
                throw love::Exception("The '%s' blend mode must be used with premultiplied alpha.", modestr);
                break;
            default:
                break;
        }
    }


    DkBlendOp func = DkBlendOp_Add;

    DkBlendFactor srcColor = DkBlendFactor_One;
    DkBlendFactor srcAlpha = DkBlendFactor_One;

    DkBlendFactor dstColor = DkBlendFactor_Zero;
    DkBlendFactor dstAlpha = DkBlendFactor_Zero;

    switch (mode)
    {
        case love::Graphics::BLEND_ALPHA:
            srcColor = srcAlpha = DkBlendFactor_One;
            dstColor = dstAlpha = DkBlendFactor_Src1Alpha;

            break;
        case love::Graphics::BLEND_MULTIPLY:
            srcColor = srcAlpha = DkBlendFactor_DstColor;
            dstColor = dstAlpha = DkBlendFactor_Zero;

            break;
        case love::Graphics::BLEND_SUBTRACT:
            func = DkBlendOp_RevSub;

            break;
        case love::Graphics::BLEND_ADD:
            srcColor = DkBlendFactor_One;
            srcAlpha = DkBlendFactor_Zero;

            dstColor = dstAlpha = DkBlendFactor_One;

            break;
        case love::Graphics::BLEND_LIGHTEN:
            func = DkBlendOp_Max;

            break;
        case love::Graphics::BLEND_DARKEN:
            func = DkBlendOp_Min;

            break;
        case love::Graphics::BLEND_SCREEN:
            srcColor = srcAlpha = DkBlendFactor_One;
            dstColor = dstAlpha = DkBlendFactor_Src1Alpha;

            break;
        case love::Graphics::BLEND_REPLACE:
        case love::Graphics::BLEND_NONE:
        default:
            srcColor = srcAlpha = DkBlendFactor_One;
            dstColor = dstAlpha = DkBlendFactor_Zero;

            break;
    }

    // We can only do alpha-multiplication when srcRGB would have been unmodified.
    if (srcColor == DkBlendFactor_One && alphamode == BLENDALPHA_MULTIPLY && mode != BLEND_NONE)
        srcColor = DkBlendFactor_SrcAlpha;

    dk3d.SetBlendMode(func, srcColor, srcAlpha, dstColor, dstAlpha);

    states.back().blendMode = mode;
    states.back().blendAlphaMode = alphamode;
}

void love::deko3d::Graphics::Rectangle(const std::string & mode, float x, float y, float width, float height, float rx, float ry)
{
    const Colorf & color = this->states.back().foreground;

    vertex::Vertex points[4] =
    {
        { {x,         y,          0}, {color.r, color.g, color.b, color.a} },
        { {x + width, y,          0}, {color.r, color.g, color.b, color.a} },
        { {x + width, y + height, 0}, {color.r, color.g, color.b, color.a} },
        { {x, y + height,         0}, {color.r, color.g, color.b, color.a} }
    };

    dk3d.RenderRectangle(mode, points);
}

void love::deko3d::Graphics::Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth)
{
    if (color.has_value() || stencil.has_value() || depth.has_value())
    {} // flush stream draws

    if (color.has_value())
    {
        Graphics::GammaCorrectColor(color.value());
        dk3d.ClearColor(color.value());
    }

    if (stencil.has_value())
        dk3d.ClearStencil(stencil.value());
}

void love::deko3d::Graphics::Present()
{
    dk3d.Present();
}

void love::deko3d::Graphics::SetScissor(const Rect & scissor)
{
    // Flush Stream Draws

    DisplayState & state = this->states.back();

    dk3d.SetScissor(scissor, false);

    state.scissor = true;
    state.scissorRect = scissor;
}

void love::deko3d::Graphics::SetColorMask(ColorMask mask)
{
    // flushStreamDraws();

    dk3d.SetColorMask(mask.r, mask.g, mask.b, mask.a);
    states.back().colorMask = mask;
}

void love::deko3d::Graphics::SetScissor()
{
    if (states.back().scissor)
    {} // Flush Stream Draws

    states.back().scissor = false;
}

Shader * love::deko3d::Graphics::NewShader(void * vertex, size_t vertex_sz, void * pixel, size_t pixel_sz)
{
    if (vertex == NULL && pixel == NULL)
        throw love::Exception("Error creating shader: no source code!");

    StrongReference<ShaderStage> vertexstage(this->NewShaderStage(ShaderStage::STAGE_VERTEX, vertex, vertex_sz), Acquire::NORETAIN);
    StrongReference<ShaderStage> pixelstage(this->NewShaderStage(ShaderStage::STAGE_PIXEL, pixel, pixel_sz), Acquire::NORETAIN);

    return new Shader(vertexstage, pixelstage);
}

ShaderStage * love::deko3d::Graphics::NewShaderStage(ShaderStage::StageType stage, void * data, size_t size)
{
    if (stage == ShaderStage::STAGE_MAX_ENUM)
        throw love::Exception("Invalid shader stage.");

    ShaderStage * s = nullptr;

    if (s == nullptr)
        return new ShaderStage(this, stage, data, size);

    return new ShaderStage(this, stage, data, size);
}