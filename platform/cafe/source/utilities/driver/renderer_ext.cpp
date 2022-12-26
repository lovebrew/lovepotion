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

#include <utilities/log/logfile.h>

static void initColorBuffer(GX2ColorBuffer& buffer, Vector2& size, GX2SurfaceFormat format)
{
    memset(&buffer, 0, sizeof(GX2ColorBuffer));

    buffer.surface.use       = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
    buffer.surface.dim       = GX2_SURFACE_DIM_TEXTURE_2D;
    buffer.surface.width     = size.x;
    buffer.surface.height    = size.y;
    buffer.surface.depth     = 1;
    buffer.surface.mipLevels = 1;
    buffer.surface.format    = format;
    buffer.surface.tileMode  = GX2_TILE_MODE_DEFAULT;
    buffer.viewNumSlices     = 1;

    GX2CalcSurfaceSizeAndAlignment(&buffer.surface);
    GX2InitColorBufferRegs(&buffer);
}

Renderer<Console::CAFE>::Renderer() :
    inForeground(false),
    commandBuffer(nullptr),
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

    auto& framebuffer = this->framebuffers[(uint8_t)Screen::SCREEN_TV];
    GX2SetTVScale(framebuffer.dimensions.x, framebuffer.dimensions.y);
    GX2SetTVEnable(true);

    framebuffer = this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD];
    GX2SetDRCScale(framebuffer.dimensions.x, framebuffer.dimensions.y);
    GX2SetDRCEnable(true);

    GX2SetDepthOnlyControl(false, false, GX2_COMPARE_FUNC_ALWAYS);
    GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, false, true);
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
    auto& framebuffer      = this->framebuffers[(uint8_t)Screen::SCREEN_TV];
    framebuffer.scanBuffer = MEMAllocFromFrmHeapEx(foregroundHeap, framebuffer.scanBufferSize,
                                                   GX2_SCAN_BUFFER_ALIGNMENT);
    if (!framebuffer.scanBuffer)
        return -1;

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, framebuffer.scanBuffer, framebuffer.scanBufferSize);
    GX2SetTVBuffer(framebuffer.scanBuffer, framebuffer.scanBufferSize,
                   (GX2TVRenderMode)framebuffer.mode, FRAMEBUFFER_FORMAT, FRAMEBUFFER_BUFFERING);

    /* allocate gamepad scan buffer */
    framebuffer            = this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD];
    framebuffer.scanBuffer = MEMAllocFromFrmHeapEx(foregroundHeap, framebuffer.scanBufferSize,
                                                   GX2_SCAN_BUFFER_ALIGNMENT);

    if (!framebuffer.scanBuffer)
        return -2;

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, framebuffer.scanBuffer, framebuffer.scanBufferSize);
    GX2SetDRCBuffer(framebuffer.scanBuffer, framebuffer.scanBufferSize,
                    (GX2DrcRenderMode)framebuffer.mode, FRAMEBUFFER_FORMAT, FRAMEBUFFER_BUFFERING);
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

    /* set up the TV */

    // clang-format off
    auto framebuffer = this->framebuffers[(uint8_t)Screen::SCREEN_TV];
    GX2CalcTVSize((GX2TVRenderMode)framebuffer.mode, FRAMEBUFFER_FORMAT, FRAMEBUFFER_BUFFERING, &framebuffer.scanBufferSize, &unknown);
    initColorBuffer(framebuffer.buffer, framebuffer.dimensions, FRAMEBUFFER_FORMAT);

    /* set up the Gamepad */

    framebuffer = this->framebuffers[(uint8_t)Screen::SCREEN_GAMEPAD];
    GX2CalcDRCSize((GX2DrcRenderMode)framebuffer.mode, FRAMEBUFFER_FORMAT, FRAMEBUFFER_BUFFERING, &framebuffer.scanBufferSize, &unknown);
    initColorBuffer(framebuffer.buffer, framebuffer.dimensions, FRAMEBUFFER_FORMAT);
    // clang-format on
}

void Renderer<Console::CAFE>::DestroyFramebuffers()
{}

void Renderer<Console::CAFE>::EnsureInFrame()
{
    if (!this->inFrame)
    {
        this->inFrame = true;
    }
}

void Renderer<Console::CAFE>::Clear(const Color& color)
{
    GX2ClearColor(&this->current.buffer, color.r, color.g, color.b, color.a);
    GX2SetContextState(this->state);
}

void Renderer<Console::CAFE>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{}

void Renderer<Console::CAFE>::SetBlendColor(const Color& color)
{}

void Renderer<Console::CAFE>::BindFramebuffer(/* Canvas* canvas */)
{
    this->EnsureInFrame();

    const auto activeScreenId = (uint8_t)Graphics<>::activeScreen;

    if (activeScreenId < 0)
        return;

    this->current = this->framebuffers[activeScreenId];
    GX2SetColorBuffer(&this->current.buffer, GX2_RENDER_TARGET_0);
}

void Renderer<Console::CAFE>::Present()
{
    std::optional<GX2ScanTarget> target;
    target = Renderer::scanBuffers.Find(Graphics<>::activeScreen);

    GX2CopyColorBufferToScanBuffer(&this->current.buffer, *target);
    GX2SetContextState(this->state);

    GX2SwapScanBuffers();
    GX2SetContextState(this->state);

    GX2Flush();

    /* wait to flip */
    {
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
    }
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
{}

void Renderer<Console::CAFE>::SetViewport(const Rect& viewport)
{
    GX2SetViewport(viewport.x, viewport.y, viewport.w, viewport.h, Z_NEAR, Z_FAR);

    const auto ortho = glm::ortho(0.0f, (float)viewport.w, (float)viewport.h, 0.0f, Z_NEAR, Z_FAR);
    this->transform.projection = ortho;
}

void Renderer<Console::CAFE>::SetScissor(const Rect& scissor, bool canvasActive)
{
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
