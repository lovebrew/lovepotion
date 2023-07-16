#include <utilities/driver/renderer_ext.hpp>

#include <common/exception.hpp>
#include <common/luax.hpp>

#include <objects/shader_ext.hpp>
#include <objects/texture_ext.hpp>

using namespace love;

Renderer<Console::CTR>::Renderer() : targets {}, current(nullptr)
{
    gfxInitDefault();
    gfxSet3D(true);

    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
        throw love::Exception("Failed to initialize citro3d!");

    C3D_CullFace(GPU_CULL_NONE);
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

    C3D_AttrInfo* attributes = C3D_GetAttrInfo();
    AttrInfo_Init(attributes);

    AttrInfo_AddLoader(attributes, 0, GPU_FLOAT, 3); // position
    AttrInfo_AddLoader(attributes, 1, GPU_FLOAT, 4); // color
    AttrInfo_AddLoader(attributes, 2, GPU_FLOAT, 2); // texcoord
}

Renderer<Console::CTR>::~Renderer()
{
    C3D_Fini();
    gfxExit();
}

Renderer<Console::CTR>::Info Renderer<Console::CTR>::GetRendererInfo()
{
    if (this->info.filled)
        return this->info;

    this->info.device  = Renderer::RENDERER_DEVICE;
    this->info.name    = Renderer::RENDERER_NAME;
    this->info.vendor  = Renderer::RENDERER_VENDOR;
    this->info.version = Renderer::RENDERER_VERSION;

    this->info.filled = true;

    return this->info;
}

void Renderer<Console::CTR>::CreateFramebuffers()
{
    for (uint8_t index = 0; index < this->targets.size(); index++)
        this->targets[index].Create((Screen)index);
}

void Renderer<Console::CTR>::DestroyFramebuffers()
{
    for (uint8_t index = 0; index < this->targets.size(); index++)
        this->targets[index].Destroy();
}

void Renderer<Console::CTR>::Clear(const Color& color)
{
    C3D_RenderTargetClear(this->current->GetTarget(), C3D_CLEAR_ALL, color.abgr(), 0);
}

/* todo */
void Renderer<Console::CTR>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{}

/* kept track of in Graphics<Console::CTR> */
void Renderer<Console::CTR>::SetBlendColor(const Color& color)
{}

void Renderer<Console::CTR>::EnsureInFrame()
{
    if (!this->inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        this->commands.clear();
        this->inFrame = true;
    }
}

void Renderer<Console::CTR>::BindFramebuffer(Texture<Console::CTR>* texture)
{
    if (!IsActiveScreenValid())
        return;

    this->EnsureInFrame();

    if (texture != nullptr && texture->IsRenderTarget())
    {
        // this->current = texture->GetHandle();
        this->SetViewport({ 0, 0, texture->GetPixelWidth(), texture->GetPixelHeight() });
        this->SetScissor({ 0, 0, texture->GetPixelWidth(), texture->GetPixelHeight() }, true);
    }
    else
    {
        this->current = &this->targets[love::GetActiveScreen()];

        this->SetViewport(this->current->GetViewport());
        this->SetScissor(this->current->GetScissor(), false);
    }

    C3D_FrameDrawOn(this->current->GetTarget());
}

bool Renderer<Console::CTR>::Render(DrawCommand<Console::CTR>& command)
{
    Shader<Console::CTR>::defaults[command.shader]->Attach();

    if (!command.buffer->IsValid())
        return false;

    auto uniforms = Shader<Console::CTR>::current->GetUniformLocations();
    this->current->UseProjection(uniforms);

    // if (command.handles.size() > 0)
    // {
    //     if (this->CheckHandle(command.handles.back()))
    //         C3D_TexBind(0, this->currentTexture);
    // }

    std::optional<GPU_Primitive_t> primitive;
    if (!(primitive = Renderer::primitiveModes.Find(command.type)))
        return false;

    C3D_DrawArrays(*primitive, 0, command.count);
    this->commands.push_back(command.buffer);

    return true;
}

void Renderer<Console::CTR>::Present()
{
    if (this->inFrame)
    {
        C3D_FrameEnd(0);
        this->inFrame = false;
    }

    for (size_t i = this->deferred.size(); i > 0; i--)
    {
        this->deferred[i - 1]();
        this->deferred.erase(deferred.begin() + i - 1);
    }
}

void Renderer<Console::CTR>::SetViewport(const Rect& viewport)
{
    this->current->SetViewport(viewport);
}

void Renderer<Console::CTR>::SetScissor(const Rect& scissor, bool canvasActive)
{
    this->current->SetScissor(scissor);
}

void Renderer<Console::CTR>::SetStencil(RenderState::CompareMode mode, int value)
{
    bool enabled = (mode == RenderState::COMPARE_ALWAYS) ? false : true;

    std::optional<GPU_TESTFUNC> compareOp;
    if (!(compareOp = Renderer::compareModes.Find(mode)))
        return;

    C3D_StencilTest(enabled, *compareOp, value, 0xFFFFFFFF, 0xFFFFFFFF);
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
}

void Renderer<Console::CTR>::SetMeshCullMode(vertex::CullMode mode)
{
    std::optional<GPU_CULLMODE> cullMode;
    if (!(cullMode = Renderer::cullModes.Find(mode)))
        return;

    C3D_CullFace(*cullMode);
}

/* ??? */
void Renderer<Console::CTR>::SetVertexWinding(vertex::Winding winding)
{}

void Renderer<Console::CTR>::SetSamplerState(Texture<Console::CTR>* texture, SamplerState& state)
{
    /* set the min and mag filters */

    auto* handle = texture->GetHandle();

    std::optional<GPU_TEXTURE_FILTER_PARAM> mag;
    if (!(mag = Renderer::filterModes.Find(state.magFilter)))
        return;

    std::optional<GPU_TEXTURE_FILTER_PARAM> min;
    if (!(min = Renderer::filterModes.Find(state.minFilter)))
        return;

    C3D_TexSetFilter(handle, *mag, *min);

    /* set the wrapping modes */

    std::optional<GPU_TEXTURE_WRAP_PARAM> wrapU;
    if (!(wrapU = Renderer::wrapModes.Find(state.wrapU)))
        return;

    std::optional<GPU_TEXTURE_WRAP_PARAM> wrapV;
    if (!(wrapV = Renderer::wrapModes.Find(state.wrapV)))
        return;

    C3D_TexSetWrap(handle, *wrapU, *wrapV);
}

void Renderer<Console::CTR>::SetColorMask(const RenderState::ColorMask& mask)
{
    uint8_t writeMask = GPU_WRITE_DEPTH;
    writeMask |= mask.GetColorMask();

    C3D_DepthTest(true, GPU_GEQUAL, (GPU_WRITEMASK)writeMask);
}

void Renderer<Console::CTR>::SetBlendMode(const RenderState::BlendState& state)
{
    std::optional<GPU_BLENDEQUATION> opRGB;
    if (!(opRGB = Renderer::blendEquations.Find(state.operationRGB)))
        return;

    std::optional<GPU_BLENDEQUATION> opAlpha;
    if (!(opAlpha = Renderer::blendEquations.Find(state.operationA)))
        return;

    std::optional<GPU_BLENDFACTOR> srcColor;
    if (!(srcColor = Renderer::blendFactors.Find(state.srcFactorRGB)))
        return;

    std::optional<GPU_BLENDFACTOR> dstColor;
    if (!(dstColor = Renderer::blendFactors.Find(state.dstFactorRGB)))
        return;

    std::optional<GPU_BLENDFACTOR> srcAlpha;
    if (!(srcAlpha = Renderer::blendFactors.Find(state.srcFactorA)))
        return;

    std::optional<GPU_BLENDFACTOR> dstAlpha;
    if (!(dstAlpha = Renderer::blendFactors.Find(state.dstFactorA)))
        return;

    C3D_AlphaBlend(*opRGB, *opAlpha, *srcColor, *dstColor, *srcAlpha, *dstAlpha);
}
