#include <utilities/driver/renderer_ext.hpp>
#include <utilities/driver/vertex_ext.hpp>

#include <common/exception.hpp>
#include <common/luax.hpp>

#include <algorithm>

#include <objects/shader_ext.hpp>
#include <objects/texture_ext.hpp>

using namespace love;

Renderer<Console::CTR>::Renderer() : targets {}, currentTexture(nullptr)
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

    BufInfo_Init(&this->bufferInfo);
    m_vertices = (Vertex*)linearAlloc(TOTAL_BUFFER_SIZE);

    if (!m_vertices)
        throw love::Exception("Out of memory.");

    int result = BufInfo_Add(&this->bufferInfo, (void*)m_vertices, VERTEX_SIZE, 0x03, 0x210);
    C3D_SetBufInfo(&this->bufferInfo);

    if (result < 0)
        throw love::Exception("Failed to add C3D_BufInfo.");

    Mtx_Identity(&this->context.projection);
    Mtx_Identity(&this->context.modelView);
}

Renderer<Console::CTR>::~Renderer()
{
    linearFree(m_vertices);

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
    C3D_FrameSplit(0);
    C3D_RenderTargetClear(this->context.target, C3D_CLEAR_ALL, color.abgr(), 0);
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
        this->inFrame = true;
    }
}

void Renderer<Console::CTR>::BindFramebuffer(Texture<Console::ALL>* texture)
{
    if (!IsActiveScreenValid())
        return;

    this->EnsureInFrame();
    FlushVertices();

    this->context.target = this->targets[love::GetActiveScreen()].GetTarget();
    Rect viewport        = this->targets[love::GetActiveScreen()].GetViewport();

    if (texture != nullptr && texture->IsRenderTarget())
    {
        auto* _texture       = (Texture<Console::CTR>*)texture;
        this->context.target = _texture->GetRenderTargetHandle();

        viewport = { 0, 0, _texture->GetPixelWidth(), _texture->GetPixelHeight() };
    }

    C3D_FrameDrawOn(this->context.target);
    this->SetViewport(viewport, this->context.target->linked);
}

void Renderer<Console::CTR>::FlushVertices()
{
    for (const auto& command : m_commands)
    {
        std::memcpy(m_vertices + m_vertexOffset, command.Vertices().get(), command.size);

        if (m_format != command.format)
        {
            const auto setTexEnvFunction = vertex::attributes::GetTexEnvFunction(command.format);
            setTexEnvFunction();

            m_format = command.format;
        }

        std::optional<GPU_Primitive_t> primitive;
        if (!(primitive = primitiveModes.Find(command.type)))
            throw love::Exception("Invalid primitive mode");

        ++drawCallsBatched;
        C3D_DrawArrays(*primitive, m_vertexOffset, command.count);
        m_vertexOffset += command.count;
    }

    m_commands.clear();
}

bool Renderer<Console::CTR>::Render(DrawCommand<Console::CTR>& command)
{
    {
        Shader<Console::CTR>::defaults[command.shader]->Attach();

        auto uniforms = Shader<Console::CTR>::current->GetUniformLocations();

        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uniforms.uLocProjMtx, &this->context.projection);
        C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uniforms.uLocMdlView, &this->context.modelView);
    }

    if ((command.count + m_vertexOffset) * VERTEX_SIZE > TOTAL_BUFFER_SIZE)
    {
        FlushVertices();
        m_vertexOffset = 0;
    }

    // check if texture is the same, or no texture at all
    if (command.handles.empty() ||
        (command.handles.size() > 0 && this->currentTexture == command.handles.back()))
    {
        ++drawCalls;
        m_commands.push_back(command.Clone());
        return true;
    }
    else
    {
        FlushVertices();

        if (!command.handles.empty())
        {
            if (this->currentTexture != command.handles.back())
                this->currentTexture = command.handles.back();

            C3D_TexBind(0, command.handles.back());
        }
        ++drawCalls;
        m_commands.push_back(command.Clone());
        return true;
    }

    return false;
}

void Renderer<Console::CTR>::Present()
{
    if (this->inFrame)
    {
        FlushVertices();
        C3D_FrameEnd(0);

        m_vertexOffset = 0;

        this->inFrame = false;
    }

    Renderer<>::cpuTime = C3D_GetProcessingTime();
    Renderer<>::gpuTime = C3D_GetDrawingTime();

    for (size_t i = this->deferred.size(); i > 0; i--)
    {
        this->deferred[i - 1]();
        this->deferred.erase(deferred.begin() + i - 1);
    }
}

void Renderer<Console::CTR>::SetViewport(const Rect& rect, bool tilt)
{
    Rect newView = rect;

    if (newView.h == GSP_SCREEN_WIDTH && tilt)
    {
        if (newView.w == GSP_SCREEN_HEIGHT_TOP || newView.w == GSP_SCREEN_HEIGHT_TOP_2X)
        {
            Mtx_Copy(&this->context.projection, &this->targets[0].GetProjView());
            return;
        }
        else if (newView.w == GSP_SCREEN_HEIGHT_BOTTOM)
        {
            Mtx_Copy(&this->context.projection, &this->targets[2].GetProjView());
            return;
        }
    }

    auto* ortho = tilt ? Mtx_OrthoTilt : Mtx_Ortho;
    ortho(&this->context.projection, 0.0f, rect.w, rect.h, 0.0f, Z_NEAR, Z_FAR, true);

    C3D_SetViewport(0, 0, rect.w, rect.h);
}

void Renderer<Console::CTR>::SetScissor(const Rect& scissor, bool canvasActive)
{
    this->targets[love::GetActiveScreen()].SetScissor(scissor, canvasActive);
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
