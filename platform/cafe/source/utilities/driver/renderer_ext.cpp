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

static void initColorBuffer(GX2ColorBuffer& buffer, const Vector2& size, GX2SurfaceFormat format)
{
    memset(&buffer, 0, sizeof(GX2ColorBuffer));

    buffer.surface.use       = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
    buffer.surface.dim       = GX2_SURFACE_DIM_TEXTURE_2D;
    buffer.surface.aa        = GX2_AA_MODE1X;
    buffer.surface.width     = size.x;
    buffer.surface.height    = size.y;
    buffer.surface.depth     = 1;
    buffer.surface.mipLevels = 1;
    buffer.surface.format    = format;
    buffer.surface.swizzle   = 0;
    buffer.surface.tileMode  = GX2_TILE_MODE_DEFAULT;
    buffer.surface.mipmaps   = nullptr;
    buffer.viewFirstSlice    = 0;
    buffer.viewMip           = 0;
    buffer.viewNumSlices     = 1;

    GX2CalcSurfaceSizeAndAlignment(&buffer.surface);
    GX2InitColorBufferRegs(&buffer);
}

Renderer<Console::CAFE>::Renderer() :
    inForeground(false),
    commandBuffer(nullptr),
    current(nullptr),
    state {},
    framebuffers()
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

    this->transform.modelView = glm::mat4(1.0f);

    this->CreateFramebuffers();

    ProcUIRegisterCallback(PROCUI_CALLBACK_ACQUIRE, ProcUIAcquired, nullptr, 100);
    ProcUIRegisterCallback(PROCUI_CALLBACK_RELEASE, ProcUIReleased, nullptr, 100);

    if (int result = this->OnForegroundAcquired(); result != 0)
        throw love::Exception("Failed to acquire application foreground (error %d).", result);

    this->state = (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

    if (!this->state)
        throw love::Exception("Failed to create GX2ContextState.");

    GX2SetupContextStateEx(this->state, true);
    GX2SetContextState(this->state);

    GX2SetTVScale(this->framebuffers[(uint8_t)Screen::SCREEN_TV].dimensions.x,
                  this->framebuffers[(uint8_t)Screen::SCREEN_TV].dimensions.y);
    GX2SetTVEnable(true);

    GX2SetDRCScale(this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].dimensions.x,
                   this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].dimensions.y);
    GX2SetDRCEnable(true);

    GX2SetDepthOnlyControl(true, false, GX2_COMPARE_FUNC_ALWAYS);

    GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, true, true);

    auto state = RenderState::ComputeBlendState(RenderState::BLEND_ALPHA,
                                                RenderState::BLENDALPHA_PREMULTIPLIED);
    this->SetBlendMode(state);

    GX2SetSwapInterval(1);
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
    this->framebuffers[(uint8_t)Screen::SCREEN_TV].scanBuffer = MEMAllocFromFrmHeapEx(
        foregroundHeap, this->framebuffers[(uint8_t)Screen::SCREEN_TV].scanBufferSize,
        GX2_SCAN_BUFFER_ALIGNMENT);

    if (!this->framebuffers[(uint8_t)Screen::SCREEN_TV].scanBuffer)
        return -1;

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU,
                  this->framebuffers[(uint8_t)Screen::SCREEN_TV].scanBuffer,
                  this->framebuffers[(uint8_t)Screen::SCREEN_TV].scanBufferSize);

    GX2SetTVBuffer(this->framebuffers[(uint8_t)Screen::SCREEN_TV].scanBuffer,
                   this->framebuffers[(uint8_t)Screen::SCREEN_TV].scanBufferSize,
                   (GX2TVRenderMode)this->framebuffers[(uint8_t)Screen::SCREEN_TV].mode,
                   FRAMEBUFFER_FORMAT, FRAMEBUFFER_BUFFERING);

    /* allocate gamepad scan buffer */
    this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].scanBuffer = MEMAllocFromFrmHeapEx(
        foregroundHeap, this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].scanBufferSize,
        GX2_SCAN_BUFFER_ALIGNMENT);

    if (!this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].scanBuffer)
        return -2;

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU,
                  this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].scanBuffer,
                  this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].scanBufferSize);

    GX2SetDRCBuffer(this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].scanBuffer,
                    this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].scanBufferSize,
                    (GX2DrcRenderMode)this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].mode,
                    FRAMEBUFFER_FORMAT, FRAMEBUFFER_BUFFERING);

    /* allocate color buffers */

    for (int index = 0; index < Renderer::MAX_RENDERTARGETS; index++)
    {
        auto& buffer = this->framebuffers[index].buffer;

        const auto size      = buffer.surface.imageSize;
        const auto alignment = buffer.surface.alignment;

        buffer.surface.image = MEMAllocFromFrmHeapEx(memOneHeap, size, alignment);

        if (!buffer.surface.image)
            return -3;

        GX2Invalidate(INVALIDATE_COLOR_BUFFER, buffer.surface.image, buffer.surface.imageSize);
    }

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

void Renderer<Console::CAFE>::CreateFramebuffers()
{
    switch (GX2GetSystemTVScanMode())
    {
        case GX2_TV_SCAN_MODE_480I:
        case GX2_TV_SCAN_MODE_480P:
        {
            this->framebuffers[(uint8_t)Screen::SCREEN_TV].mode = GX2_TV_RENDER_MODE_WIDE_480P;
            this->framebuffers[(uint8_t)Screen::SCREEN_TV].dimensions = Vector2(854, 480);
            break;
        }
        case GX2_TV_SCAN_MODE_720P:
        {
            this->framebuffers[(uint8_t)Screen::SCREEN_TV].mode = GX2_TV_RENDER_MODE_WIDE_720P;
            this->framebuffers[(uint8_t)Screen::SCREEN_TV].dimensions = Vector2(1280, 720);
            break;
        }
        case GX2_TV_SCAN_MODE_1080I:
        case GX2_TV_SCAN_MODE_1080P:
        default:
        {
            this->framebuffers[(uint8_t)Screen::SCREEN_TV].mode = GX2_TV_RENDER_MODE_WIDE_1080P;
            this->framebuffers[(uint8_t)Screen::SCREEN_TV].dimensions = Vector2(1920, 1080);
            break;
        }
    }

    this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].mode       = GX2_DRC_RENDER_MODE_SINGLE;
    this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].dimensions = Vector2(854, 480);

    uint32_t unknown = 0;

    // clang-format off

    /* set up the TV */
    GX2CalcTVSize((GX2TVRenderMode)this->framebuffers[(uint8_t)Screen::SCREEN_TV].mode, FRAMEBUFFER_FORMAT, FRAMEBUFFER_BUFFERING,
                  &this->framebuffers[(uint8_t)Screen::SCREEN_TV].scanBufferSize, &unknown);

    initColorBuffer(this->framebuffers[(uint8_t)Screen::SCREEN_TV].buffer, this->framebuffers[(uint8_t)Screen::SCREEN_TV].dimensions, FRAMEBUFFER_FORMAT);

    /* set up the Gamepad */
    GX2CalcDRCSize((GX2DrcRenderMode)this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].mode, FRAMEBUFFER_FORMAT, FRAMEBUFFER_BUFFERING,
                   &this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].scanBufferSize, &unknown);

    initColorBuffer(this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].buffer, this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD].dimensions, FRAMEBUFFER_FORMAT);
    // clang-format on
}

void Renderer<Console::CAFE>::DestroyFramebuffers()
{}

void Renderer<Console::CAFE>::EnsureInFrame()
{
    const auto activeScreenId = (uint8_t)Graphics<>::activeScreen;
    this->current             = &this->framebuffers[activeScreenId].buffer;
}

void Renderer<Console::CAFE>::Clear(const Color& color)
{
    GX2ClearColor(this->current, color.r, color.g, color.b, color.a);
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{}

void Renderer<Console::CAFE>::SetBlendColor(const Color& color)
{}

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
{}

void Renderer<Console::CAFE>::SetVertexWinding(vertex::Winding winding)
{
    std::optional<GX2FrontFace> face;
    if (!(face = Renderer::windingModes.Find(winding)))
        return;

    GX2SetCullOnlyControl(*face, true, true);
}

static Renderer<Console::CAFE>::Transform swapEndianness(
    const Renderer<Console::CAFE>::Transform transform)
{
    Renderer<Console::CAFE>::Transform result {};

    uint* dstModel = (uint*)glm::value_ptr(result.modelView);
    uint* dstProj  = (uint*)glm::value_ptr(result.projection);

    uint* model = (uint*)glm::value_ptr(transform.modelView);
    for (size_t index = 0; index < 16; index++)
        dstModel[index] = __builtin_bswap32(model[index]);

    uint* projection = (uint*)glm::value_ptr(transform.projection);
    for (size_t index = 0; index < 16; index++)
        dstProj[index] = __builtin_bswap32(projection[index]);

    return result;
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
        GX2SetColorBuffer(this->current, GX2_RENDER_TARGET_0);

        this->SetViewport(Rect::EMPTY);
        this->SetScissor(Rect::EMPTY);
    }

    this->littleTransform = swapEndianness(this->transform);
    GX2SetVertexUniformBlock(1, TRANSFORM_SIZE, &this->littleTransform);
}

void Renderer<Console::CAFE>::Present()
{
    GX2CopyColorBufferToScanBuffer(&this->framebuffers[0].buffer, GX2_SCAN_TARGET_TV);
    GX2CopyColorBufferToScanBuffer(&this->framebuffers[1].buffer, GX2_SCAN_TARGET_DRC);

    GX2SwapScanBuffers();
    GX2SetContextState(this->state);

    GX2Flush();

    /* wait to flip */
    uint32_t swaps, flips;
    OSTime lastFlip, lastVsync;
    uint32_t waitCount;

    while (true)
    {
        GX2GetSwapStatus(&swaps, &flips, &lastFlip, &lastVsync);

        if (flips >= swaps)
            break;

        /* GPU timed out */
        if (waitCount >= 0)
            break;

        waitCount++;
        GX2WaitForVsync();
    }

    for (auto& buffer : this->buffers)
        GX2RDestroyBufferEx(&buffer, GX2R_RESOURCE_BIND_NONE);
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

const Vector2& Renderer<Console::CAFE>::GetFrameBufferSize(Screen screen)
{
    return this->framebuffers[(uint8_t)screen].dimensions;
}

void Renderer<Console::CAFE>::UseProgram(const WHBGfxShaderGroup& group)
{
    GX2SetFetchShader(&group.fetchShader);
    GX2SetVertexShader(group.vertexShader);
    GX2SetPixelShader(group.pixelShader);

    this->littleTransform = swapEndianness(this->transform);
    GX2SetVertexUniformBlock(1, TRANSFORM_SIZE, &this->littleTransform);
}

bool Renderer<Console::CAFE>::Render(Graphics<Console::CAFE>::DrawCommand& command)
{
    Shader<Console::CAFE>::defaults[command.shader]->Attach();

    std::optional<GX2PrimitiveMode> primitive;
    if (!(primitive = primitiveModes.Find(command.primitveType)))
        return false;

    if (!command.handles.empty())
    {
        for (size_t index = 0; index < command.handles.size(); index++)
        {
            uint32_t location = Shader<Console::CAFE>::current->GetPixelSamplerLocation(index);

            GX2SetPixelTexture(command.handles[index]->GetHandle(), location);
            GX2SetPixelSampler(&command.handles[index]->GetSampler(), location);
        }
    }

    GX2RSetAttributeBuffer(&command.buffer, 0, command.buffer.elemSize, 0);
    GX2DrawEx(*primitive, command.buffer.elemCount, 0, 1);

    this->buffers.push_back(command.buffer);

    return true;
}

void Renderer<Console::CAFE>::SetViewport(const Rect& viewport)
{
    this->EnsureInFrame();

    glm::highp_mat4 ortho {};

    if (viewport == Rect::EMPTY)
    {
        const auto& buffer = this->current->surface;
        GX2SetViewport(0, 0, buffer.width, buffer.height, Z_NEAR, Z_FAR);

        ortho = glm::ortho(0.0f, (float)buffer.width, (float)buffer.height, 0.0f, Z_NEAR, Z_FAR);
    }
    else
    {
        GX2SetViewport(viewport.x, viewport.y, viewport.w, viewport.h, Z_NEAR, Z_FAR);
        ortho = glm::ortho(0.0f, (float)viewport.w, (float)viewport.h, 0.0f, Z_NEAR, Z_FAR);
    }

    this->transform.projection = ortho;
}

void Renderer<Console::CAFE>::SetScissor(const Rect& scissor)
{
    if (scissor == Rect::EMPTY)
    {
        const auto& buffer = this->current->surface;
        GX2SetScissor(0, 0, buffer.width, buffer.height);
    }
    else
        GX2SetScissor(scissor.x, scissor.y, scissor.w, scissor.h);
}

std::optional<Screen> Renderer<Console::CAFE>::CheckScreen(const char* name) const
{
    return gfxScreens.Find(name);
}

SmallTrivialVector<const char*, 2> Renderer<Console::CAFE>::GetScreens() const
{
    return gfxScreens.GetNames();
}
