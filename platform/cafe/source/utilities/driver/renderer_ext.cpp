#include <utilities/driver/renderer_ext.hpp>

#include <modules/keyboard_ext.hpp>

#include <objects/texture_ext.hpp>

#include <nn/swkbd.h>

#include <coreinit/memfrmheap.h>
#include <proc_ui/procui.h>

#include <malloc.h>
#include <stdlib.h>

using namespace love;

#define Keyboard() (Module::GetInstance<Keyboard<Console::CAFE>>(Module::M_KEYBOARD))

Renderer<Console::CAFE>::Renderer() :
    renderState {},
    inForeground(false),
    commandBuffer(nullptr),
    current {},
    state(nullptr),
    framebuffers {}
{
    this->commandBuffer = memalign(GX2_COMMAND_BUFFER_ALIGNMENT, GX2_COMMAND_BUFFER_SIZE);

    if (!this->commandBuffer)
        throw love::Exception("Failed to allocate command buffer.");

    // clang-format off
    uint32_t attributes[9] =
    {
        GX2_INIT_CMD_BUF_BASE, (uintptr_t)this->commandBuffer,
        GX2_INIT_CMD_BUF_POOL_SIZE, GX2_COMMAND_BUFFER_SIZE,
        GX2_INIT_ARGC, 0, GX2_INIT_ARGV, 0,
        GX2_INIT_END
    };
    // clang-format on

    GX2Init(attributes);

    for (const auto screen : love::GetScreenEnums())
        this->framebuffers[screen].Create(screen);

    this->state = (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

    if (!this->state)
        throw love::Exception("Failed to create GX2ContextState.");

    GX2SetupContextStateEx(this->state, false);
    GX2SetContextState(this->state);

    GX2SetDepthOnlyControl(false, false, GX2_COMPARE_FUNC_ALWAYS);
    GX2SetAlphaTest(true, GX2_COMPARE_FUNC_GREATER, 0);

    GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, false, true);
    GX2SetSwapInterval(1);

    ProcUIRegisterCallback(PROCUI_CALLBACK_ACQUIRE, ProcUIAcquired, nullptr, 100);
    ProcUIRegisterCallback(PROCUI_CALLBACK_RELEASE, ProcUIReleased, nullptr, 100);

    if (int result = this->OnForegroundAcquired(); result != 0)
        throw love::Exception("Failed to acquire application foreground (error %d).", result);

    if (Keyboard() != nullptr)
        Keyboard()->Initialize();

    /* set up some state information */
    this->renderState.winding     = GX2_FRONT_FACE_CCW;
    this->renderState.cullBack    = true;
    this->renderState.depthTest   = false;
    this->renderState.depthWrite  = false;
    this->renderState.compareMode = GX2_COMPARE_FUNC_ALWAYS;
}

uint32_t Renderer<Console::CAFE>::ProcUIAcquired(void* arg)
{
    return Renderer<Console::CAFE>::Instance().OnForegroundAcquired();
}

uint32_t Renderer<Console::CAFE>::ProcUIReleased(void* arg)
{
    return Renderer<Console::CAFE>::Instance().OnForegroundReleased();
}

int Renderer<Console::CAFE>::OnForegroundAcquired()
{
    this->inForeground = true;

    auto foregroundHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
    auto memOneHeap     = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

    /* allocate tv scan buffer */
    if (!this->framebuffers[Screen::TV].AllocateScanBuffer(foregroundHeap))
        return -1;

    /* allocate gamepad scan buffer */
    if (!this->framebuffers[Screen::GAMEPAD].AllocateScanBuffer(foregroundHeap))
        return -2;

    /* invalidate tv color buffer */
    if (!this->framebuffers[Screen::TV].InvalidateColorBuffer(memOneHeap))
        return -4;

    /* invalidate gamepad color buffer */
    if (!this->framebuffers[Screen::GAMEPAD].InvalidateColorBuffer(memOneHeap))
        return -5;

    /* invalidate tv depth buffer */
    if (!this->framebuffers[Screen::TV].InvalidateDepthBuffer(memOneHeap))
        return -6;

    /* invalidate gamepad depth buffer */
    if (!this->framebuffers[Screen::GAMEPAD].InvalidateDepthBuffer(memOneHeap))
        return -7;

    return 0;
}

int Renderer<Console::CAFE>::OnForegroundReleased()
{
    auto foregroundHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
    auto memOneHeap     = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

    MEMFreeToFrmHeap(foregroundHeap, MEM_FRM_HEAP_FREE_ALL);
    MEMFreeToFrmHeap(memOneHeap, MEM_FRM_HEAP_FREE_ALL);

    this->inForeground = false;

    return 0;
}

Renderer<Console::CAFE>::~Renderer()
{
    if (this->inForeground)
        this->OnForegroundReleased();

    GX2Shutdown();

    free(this->state);
    this->state = nullptr;

    free(this->commandBuffer);
    this->commandBuffer = nullptr;
}

void Renderer<Console::CAFE>::EnsureInFrame()
{
    this->current = &this->framebuffers[love::GetActiveScreen()];
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::Clear(const Color& color)
{
    GX2ClearColor(&this->current->GetBuffer(), color.r, color.g, color.b, color.a);
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::SetDepthWrites(bool enable)
{
    GX2SetDepthOnlyControl(this->renderState.depthTest, enable, this->renderState.compareMode);
    this->renderState.depthWrite = enable;
}

void Renderer<Console::CAFE>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{
    if (!this->renderState.depthWrite)
        this->SetDepthWrites(true);

    GX2ClearDepthStencilEx(&this->current->GetDepthBuffer(), depth, stencil, GX2_CLEAR_FLAGS_BOTH);
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::SetBlendColor(const Color& color)
{
    GX2SetBlendConstantColor(color.r, color.g, color.b, color.a);
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::SetBlendMode(const RenderState::BlendState& state)
{
    std::optional<GX2BlendCombineMode> opRGB;
    if (!(opRGB = Renderer::blendEquations.Find(state.operationRGB)))
        return;

    std::optional<GX2BlendCombineMode> opAlpha;
    if (!(opAlpha = Renderer::blendEquations.Find(state.operationA)))
        return;

    std::optional<GX2BlendMode> srcColor;
    if (!(srcColor = Renderer::blendFactors.Find(state.srcFactorRGB)))
        return;

    std::optional<GX2BlendMode> dstColor;
    if (!(dstColor = Renderer::blendFactors.Find(state.dstFactorRGB)))
        return;

    std::optional<GX2BlendMode> srcAlpha;
    if (!(srcAlpha = Renderer::blendFactors.Find(state.srcFactorA)))
        return;

    std::optional<GX2BlendMode> dstAlpha;
    if (!(dstAlpha = Renderer::blendFactors.Find(state.dstFactorA)))
        return;

    GX2SetBlendControl(GX2_RENDER_TARGET_0, *srcColor, *dstColor, *opRGB, true, *srcAlpha,
                       *dstAlpha, *opAlpha);
}

void Renderer<Console::CAFE>::SetMeshCullMode(vertex::CullMode mode)
{
    bool enabled = (mode != vertex::CULL_NONE);

    this->renderState.cullFront = (enabled && mode == vertex::CULL_FRONT);
    this->renderState.cullBack  = (enabled && mode == vertex::CULL_BACK);

    GX2SetCullOnlyControl(this->renderState.winding, this->renderState.cullFront,
                          this->renderState.cullBack);
}

void Renderer<Console::CAFE>::SetVertexWinding(vertex::Winding winding)
{
    std::optional<GX2FrontFace> face;
    if (!(face = Renderer::windingModes.Find(winding)))
        return;

    bool front = this->renderState.cullFront;
    bool back  = this->renderState.cullBack;

    GX2SetCullOnlyControl(*face, front, back);
    this->renderState.winding = *face;
}

void Renderer<Console::CAFE>::BindFramebuffer(Texture<Console::CAFE>* texture)
{
    this->EnsureInFrame();

    if (texture && texture->IsRenderTarget())
    {
        GX2SetColorBuffer(texture->GetFramebuffer(), GX2_RENDER_TARGET_0);

        this->SetViewport({ 0, 0, texture->GetPixelWidth(), texture->GetPixelHeight() });
        this->SetScissor({ 0, 0, texture->GetPixelWidth(), texture->GetPixelHeight() });
    }
    else
    {
        GX2SetColorBuffer(&this->current->GetBuffer(), GX2_RENDER_TARGET_0);
        GX2SetDepthBuffer(&this->current->GetDepthBuffer());

        this->SetViewport(this->current->GetViewport());
        this->SetScissor(this->current->GetScissor());
    }

    this->current->UseProjection();
}

bool Renderer<Console::CAFE>::Render(Graphics<Console::CAFE>::DrawCommand& command)
{
    Shader<Console::CAFE>::defaults[command.shader]->Attach();

    if (!command.buffer->IsValid())
        return false;

    std::optional<GX2PrimitiveMode> primitive;
    if (!(primitive = primitiveModes.Find(command.primitveType)))
        return false;

    if (!command.handles.empty())
    {
        for (size_t index = 0; index < command.handles.size(); index++)
        {
            auto* texture = command.handles[index]->GetHandle();
            auto* sampler = &command.handles[index]->GetSampler();

            Shader<Console::CAFE>::current->BindTexture(index, texture, sampler);
        }
    }

    GX2RSetAttributeBuffer(command.buffer->GetBuffer(), 0, command.stride, 0);
    GX2DrawEx(*primitive, command.count, 0, 1);

    this->buffers.push_back(command.buffer);

    return true;
}

void Renderer<Console::CAFE>::Present()
{
    GX2DrawDone();

    if (Keyboard()->IsShowing())
    {
        nn::swkbd::DrawDRC();
        this->ClearDepthStencil(0, 0xFF, 1.0);

        GX2SetContextState(this->state);
    }

    /* copy our color buffers to their scan buffers */
    for (auto& framebuffer : this->framebuffers)
        framebuffer.second.CopyScanBuffer();

    /* swap scan buffers */
    GX2SwapScanBuffers();

    /*
    ** flush again as GX2WaitForFlip
    ** will block the CPU
    */
    GX2Flush();

    GX2WaitForFlip();

    this->buffers.clear();
}

void Renderer<Console::CAFE>::SetSamplerState(Texture<Console::CAFE>* texture, SamplerState& state)
{
    auto& sampler = texture->GetSampler();

    std::optional<GX2TexXYFilterMode> minFilter;
    if (!(minFilter = Renderer::filterModes.Find(state.minFilter)))
        return;

    std::optional<GX2TexXYFilterMode> magFilter;
    if (!(magFilter = Renderer::filterModes.Find(state.magFilter)))
        return;

    GX2InitSamplerXYFilter(&sampler, *magFilter, *minFilter, GX2_TEX_ANISO_RATIO_NONE);

    std::optional<GX2TexClampMode> wrapU;
    if (!(wrapU = Renderer::wrapModes.Find(state.wrapU)))
        return;

    std::optional<GX2TexClampMode> wrapV;
    if (!(wrapV = Renderer::wrapModes.Find(state.wrapV)))
        return;

    std::optional<GX2TexClampMode> wrapW;
    if (!(wrapW = Renderer::wrapModes.Find(state.wrapW)))
        return;

    GX2InitSamplerClamping(&sampler, *wrapU, *wrapV, *wrapW);
    GX2InitSamplerLOD(&sampler, state.minLod, state.maxLod, state.lodBias);
}

void Renderer<Console::CAFE>::UseProgram(const WHBGfxShaderGroup& group)
{
    GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);
    GX2SetFetchShader(&group.fetchShader);
    GX2SetVertexShader(group.vertexShader);
    GX2SetPixelShader(group.pixelShader);

    this->current->UseProjection();
}

/* todo */
void Renderer<Console::CAFE>::SetColorMask(const RenderState::ColorMask& mask)
{
    // GX2SetTargetChannelMasks()
}

void Renderer<Console::CAFE>::SetLineWidth(float width)
{
    GX2SetLineWidth(width);
}

void Renderer<Console::CAFE>::SetPointSize(float size)
{
    GX2SetPointSize(size, size);
}

void Renderer<Console::CAFE>::SetViewport(const Rect& viewport)
{
    this->EnsureInFrame();

    float width  = viewport.w;
    float height = viewport.h;

    if (viewport == Rect::EMPTY)
    {
        width  = (float)this->current->GetWidth();
        height = (float)this->current->GetHeight();
    }

    this->current->SetViewport(viewport);

    auto ortho = glm::ortho(0.0f, width, height, 0.0f, Z_NEAR, Z_FAR);
    this->current->SetProjection(ortho);
}

void Renderer<Console::CAFE>::SetScissor(const Rect& scissor)
{
    this->current->SetScissor(scissor);
}
