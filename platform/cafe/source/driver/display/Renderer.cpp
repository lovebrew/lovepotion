#include "driver/display/Renderer.hpp"

/* keyboard needs GX2 inited first */
#include "modules/keyboard/Keyboard.hpp"

#include <gx2/clear.h>
#include <gx2/context.h>
#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/state.h>
#include <gx2/swap.h>

#include <proc_ui/procui.h>

#include <malloc.h>

namespace love
{
#define Keyboard() (Module::getInstance<Keyboard>(Module::M_KEYBOARD))

    Renderer::Renderer() :
        context {},
        inForeground(false),
        commandBuffer(nullptr),
        state(nullptr),
        targets {}
    {}

    Renderer::~Renderer()
    {
        if (this->inForeground)
            this->onForegroundReleased();

        GX2Shutdown();

        free(this->state);
        this->state = nullptr;

        free(this->commandBuffer);
        this->commandBuffer = nullptr;
    }

    int Renderer::onForegroundAcquired()
    {
        this->inForeground = true;

        auto foregroundHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
        auto memOneHeap     = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

        for (auto& target : this->targets)
        {
            if (!target.allocateScanBuffer(foregroundHeap))
                return -1;

            if (!target.invalidateColorBuffer(memOneHeap))
                return -2;

            if (!target.invalidateDepthBuffer(memOneHeap))
                return -3;
        }

        return 0;
    }

    static uint32_t ProcUIAcquired(void*)
    {
        return Renderer::getInstance().onForegroundAcquired();
    }

    int Renderer::onForegroundReleased()
    {
        auto foregroundHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
        auto memOneHeap     = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

        MEMFreeToFrmHeap(foregroundHeap, MEM_FRM_HEAP_FREE_ALL);
        MEMFreeToFrmHeap(memOneHeap, MEM_FRM_HEAP_FREE_ALL);

        this->inForeground = false;

        return 0;
    }

    static uint32_t ProcUIReleased(void*)
    {
        return Renderer::getInstance().onForegroundReleased();
    }

    void Renderer::initialize()
    {
        if (this->initialized)
            return;

        this->commandBuffer = memalign(GX2_COMMAND_BUFFER_ALIGNMENT, GX2_COMMAND_BUFFER_SIZE);

        if (!this->commandBuffer)
            throw love::Exception("Failed to allocate GX2 command buffer.");

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

        this->createFramebuffers();

        this->state =
            (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

        if (!this->state)
            throw love::Exception("Failed to allocate GX2 context state.");

        GX2SetupContextStateEx(this->state, false);
        GX2SetContextState(this->state);

        GX2SetDepthOnlyControl(false, false, GX2_COMPARE_FUNC_ALWAYS);
        GX2SetAlphaTest(false, GX2_COMPARE_FUNC_ALWAYS, 0.0f);

        GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, false, true);
        GX2SetSwapInterval(1);

        ProcUIRegisterCallback(PROCUI_CALLBACK_ACQUIRE, ProcUIAcquired, nullptr, 100);
        ProcUIRegisterCallback(PROCUI_CALLBACK_RELEASE, ProcUIReleased, nullptr, 100);

        if (auto result = this->onForegroundAcquired(); result != 0)
            throw love::Exception("Failed to acquire foreground: {:d}", result);

        if (Keyboard())
            Keyboard()->initialize();

        this->context.winding     = GX2_FRONT_FACE_CCW;
        this->context.cullBack    = true;
        this->context.depthTest   = false;
        this->context.depthWrite  = false;
        this->context.compareMode = GX2_COMPARE_FUNC_ALWAYS;

        this->initialized = true;
    }

    void Renderer::createFramebuffers()
    {
        const auto& info = love::getScreenInfo();

        for (size_t index = 0; index < info.size(); ++index)
        {
            Framebuffer framebuffer {};
            framebuffer.create(info[index]);

            this->targets[index] = std::move(framebuffer);
        }
    }

    void Renderer::destroyFramebuffers()
    {
        for (auto& target : this->targets)
            target.destroy();
    }

    void Renderer::ensureInFrame()
    {
        if (!this->inFrame)
        {
            GX2SetContextState(this->state);
            this->inFrame = true;
        }
    }

    void Renderer::clear(const Color& color)
    {
        GX2ClearColor(&this->context.target.get(), color.r, color.g, color.b, color.a);
    }

    void Renderer::clearDepthStencil(int depth, uint8_t mask, double stencil)
    {
        GX2ClearDepthStencilEx(&this->context.target.getDepth(), depth, stencil,
                               GX2_CLEAR_FLAGS_BOTH);
    }

    void Renderer::bindFramebuffer()
    {
        this->ensureInFrame();

        this->context.target = this->targets[love::currentScreen];
        auto viewport        = this->context.target.getViewport();

        GX2SetColorBuffer(&this->context.target.get(), GX2_RENDER_TARGET_0);
        this->setViewport(viewport);
    }

    void Renderer::present()
    {
        if (this->inFrame)
        {
            // GX2DrawDone();
            this->inFrame = false;
        }

        if (Keyboard()->hasTextInput())
        {
            nn::swkbd::DrawDRC();
            GX2SetContextState(this->state);
        }

        for (auto& target : this->targets)
            target.copyScanBuffer();

        GX2SwapScanBuffers();
        GX2Flush();
        GX2WaitForFlip();
    }

    void Renderer::setViewport(const Rect& viewport)
    {
        this->context.target.setViewport(viewport);
    }

    void Renderer::setScissor(const Rect& scissor)
    {
        this->context.target.setScissor(scissor);
    }

    void Renderer::setCullMode(CullMode mode)
    {
        const auto enabled = mode != CullMode::CULL_NONE;

        this->context.cullBack  = (enabled && mode == CullMode::CULL_BACK);
        this->context.cullFront = (enabled && mode == CullMode::CULL_FRONT);

        GX2SetCullOnlyControl(this->context.winding, this->context.cullBack,
                              this->context.cullFront);
    }

    void Renderer::setVertexWinding(Winding winding)
    {
        GX2FrontFace windingMode;

        if (!Renderer::getConstant(winding, windingMode))
            return;

        GX2SetCullOnlyControl(this->context.winding, this->context.cullBack,
                              this->context.cullFront);

        this->context.winding = windingMode;
    }

    void Renderer::setColorMask(ColorChannelMask mask)
    {
        const auto red   = (GX2_CHANNEL_MASK_R * mask.r);
        const auto green = (GX2_CHANNEL_MASK_G * mask.g);
        const auto blue  = (GX2_CHANNEL_MASK_B * mask.b);
        const auto alpha = (GX2_CHANNEL_MASK_A * mask.a);

        const auto value = GX2ChannelMask(red + green + blue + alpha);
        const auto NONE  = GX2ChannelMask(0);

        GX2SetTargetChannelMasks(value, NONE, NONE, NONE, NONE, NONE, NONE, NONE);
    }

    void Renderer::setBlendState(const BlendState& state)
    {
        GX2BlendCombineMode operationRGB;
        if (!Renderer::getConstant(state.operationRGB, operationRGB))
            return;

        GX2BlendCombineMode operationA;
        if (!Renderer::getConstant(state.operationA, operationA))
            return;

        GX2BlendMode sourceColor;
        if (!Renderer::getConstant(state.srcFactorRGB, sourceColor))
            return;

        GX2BlendMode destColor;
        if (!Renderer::getConstant(state.dstFactorRGB, destColor))
            return;

        GX2BlendMode sourceAlpha;
        if (!Renderer::getConstant(state.srcFactorA, sourceAlpha))
            return;

        GX2BlendMode destAlpha;
        if (!Renderer::getConstant(state.dstFactorA, destAlpha))
            return;

        GX2SetBlendControl(GX2_RENDER_TARGET_0, sourceColor, destColor, operationRGB, true,
                           sourceAlpha, destAlpha, operationA);
    }
} // namespace love
