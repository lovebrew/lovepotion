#include "driver/display/GX2.hpp"
#include "driver/display/Uniform.hpp"

/* keyboard needs GX2 inited first */
#include "modules/graphics/Shader.hpp"
#include "modules/keyboard/Keyboard.hpp"

#include <gx2/clear.h>
#include <gx2/context.h>
#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/state.h>
#include <gx2/swap.h>

#include <proc_ui/procui.h>

#include <malloc.h>

#include "utility/logfile.hpp"

namespace love
{
#define Keyboard() (Module::getInstance<Keyboard>(Module::M_KEYBOARD))

    GX2::GX2() : context {}, inForeground(false), commandBuffer(nullptr), targets {}, state(nullptr)
    {}

    GX2::~GX2()
    {
        if (this->inForeground)
            this->onForegroundReleased();

        GX2Shutdown();

        delete this->uniform;

        free(this->state);
        this->state = nullptr;

        free(this->commandBuffer);
        this->commandBuffer = nullptr;
    }

    int GX2::onForegroundAcquired()
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
        return gx2.onForegroundAcquired();
    }

    int GX2::onForegroundReleased()
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
        return gx2.onForegroundReleased();
    }

    void GX2::initialize()
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

        this->state = (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

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

        this->uniform            = (Uniform*)memalign(0x100, sizeof(Uniform));
        this->uniform->modelView = updateMatrix(glm::mat4(1.0f));

        this->bindFramebuffer(&this->targets[0].get());

        this->initialized = true;
    }

    void GX2::createFramebuffers()
    {
        const auto& info = love::getScreenInfo();

        for (size_t index = 0; index < info.size(); ++index)
        {
            Framebuffer framebuffer {};
            framebuffer.create(info[index]);

            this->targets[index] = std::move(framebuffer);
        }
    }

    GX2ColorBuffer& GX2::getInternalBackbuffer()
    {
        return this->targets[love::currentScreen].get();
    }

    GX2DepthBuffer& GX2::getInternalDepthbuffer()
    {
        return this->targets[love::currentScreen].getDepth();
    }

    GX2ColorBuffer* GX2::getFramebuffer()
    {
        return this->context.boundFramebuffer;
    }

    void GX2::destroyFramebuffers()
    {
        for (auto& target : this->targets)
            target.destroy();
    }

    void GX2::ensureInFrame()
    {
        if (!this->inFrame)
        {
            GX2SetContextState(this->state);
            this->inFrame = true;
        }
    }

    void GX2::clear(const Color& color)
    {
        GX2ClearColor(this->getFramebuffer(), color.r, color.g, color.b, color.a);
        GX2SetContextState(this->state);
    }

    void GX2::clearDepthStencil(int depth, uint8_t mask, double stencil)
    {
        // GX2ClearDepthStencilEx(&this->getInternalDepthbuffer(), depth, stencil, GX2_CLEAR_FLAGS_BOTH);
        // GX2SetContextState(this->state);
    }

    void GX2::bindFramebuffer(GX2ColorBuffer* target)
    {
        bool bindingModified = false;

        if (this->context.boundFramebuffer != target)
        {
            bindingModified                = true;
            this->context.boundFramebuffer = target;
        }

        if (bindingModified)
        {
            GX2SetColorBuffer(target, GX2_RENDER_TARGET_0);

            this->setViewport(Rect::EMPTY);
            this->setScissor(Rect::EMPTY);
        }
    }

    void GX2::setSamplerState(TextureBase* texture, const SamplerState& state)
    {
        auto sampler = (GX2Sampler*)texture->getSamplerHandle();

        GX2TexXYFilterMode minFilter;

        if (!GX2::getConstant(state.minFilter, minFilter))
            return;

        GX2TexXYFilterMode magFilter;

        if (!GX2::getConstant(state.magFilter, magFilter))
            return;

        GX2InitSamplerXYFilter(sampler, magFilter, minFilter, GX2_TEX_ANISO_RATIO_NONE);

        GX2TexClampMode wrapU;

        if (!GX2::getConstant(state.wrapU, wrapU))
            return;

        GX2TexClampMode wrapV;

        if (!GX2::getConstant(state.wrapV, wrapV))
            return;

        GX2TexClampMode wrapW;

        if (!GX2::getConstant(state.wrapW, wrapW))
            return;

        GX2InitSamplerClamping(sampler, wrapU, wrapV, wrapW);
        GX2InitSamplerLOD(sampler, state.minLod, state.maxLod, state.lodBias);
    }

    void GX2::prepareDraw(GraphicsBase* graphics)
    {
        if ((Shader*)ShaderBase::current != nullptr)
        {
            auto* shader = (Shader*)ShaderBase::current;
            shader->updateBuiltinUniforms(graphics, this->uniform);
        }
    }

    void GX2::bindTextureToUnit(TextureBase* texture, int unit)
    {
        if (texture == nullptr)
            return;

        auto* handle = (GX2Texture*)texture->getHandle();

        if (handle == nullptr)
            return;

        auto* sampler = (GX2Sampler*)texture->getSamplerHandle();

        this->bindTextureToUnit(handle, sampler, unit);
    }

    void GX2::bindTextureToUnit(GX2Texture* texture, GX2Sampler* sampler, int unit)
    {
        auto* shader  = (Shader*)ShaderBase::current;
        auto location = shader->getPixelSamplerLocation(0);

        GX2SetPixelTexture(texture, unit);
        GX2SetPixelSampler(sampler, location);
    }

    void GX2::present()
    {
        if (this->inFrame)
        {
            Graphics::flushBatchedDrawsGlobal();

            GX2DrawDone();

            Graphics::advanceStreamBuffersGlobal();
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

    void GX2::setViewport(const Rect& viewport)
    {
        bool isEmptyViewport = viewport == Rect::EMPTY;

        const int x = isEmptyViewport ? 0 : viewport.x;
        const int y = isEmptyViewport ? 0 : viewport.y;

        const int width  = isEmptyViewport ? this->context.boundFramebuffer->surface.width : viewport.w;
        const int height = isEmptyViewport ? this->context.boundFramebuffer->surface.height : viewport.h;

        GX2SetViewport(x, y, width, height, Framebuffer::Z_NEAR, Framebuffer::Z_FAR);

        auto ortho = glm::ortho(x, width, height, y, (int)Framebuffer::Z_NEAR, (int)Framebuffer::Z_FAR);
        this->uniform->projection = updateMatrix(ortho);

        this->context.viewport = viewport;
    }

    void GX2::setScissor(const Rect& scissor)
    {
        bool isEmptyScissor = scissor == Rect::EMPTY;

        const int x = isEmptyScissor ? 0 : scissor.x;
        const int y = isEmptyScissor ? 0 : scissor.y;

        const int width  = isEmptyScissor ? this->context.boundFramebuffer->surface.width : scissor.w;
        const int height = isEmptyScissor ? this->context.boundFramebuffer->surface.height : scissor.h;

        GX2SetScissor(x, y, width, height);
        this->context.scissor = scissor;
    }

    void GX2::setCullMode(CullMode mode)
    {
        const auto enabled = mode != CullMode::CULL_NONE;

        this->context.cullBack  = (enabled && mode == CullMode::CULL_BACK);
        this->context.cullFront = (enabled && mode == CullMode::CULL_FRONT);

        GX2SetCullOnlyControl(this->context.winding, this->context.cullBack, this->context.cullFront);
    }

    void GX2::setVertexWinding(Winding winding)
    {
        GX2FrontFace windingMode;

        if (!GX2::getConstant(winding, windingMode))
            return;

        GX2SetCullOnlyControl(windingMode, this->context.cullBack, this->context.cullFront);
        this->context.winding = windingMode;
    }

    void GX2::setColorMask(ColorChannelMask mask)
    {
        const auto red   = (GX2_CHANNEL_MASK_R * mask.r);
        const auto green = (GX2_CHANNEL_MASK_G * mask.g);
        const auto blue  = (GX2_CHANNEL_MASK_B * mask.b);
        const auto alpha = (GX2_CHANNEL_MASK_A * mask.a);

        const auto value = GX2ChannelMask(red + green + blue + alpha);
        const auto NONE  = GX2ChannelMask(0);

        GX2SetTargetChannelMasks(value, NONE, NONE, NONE, NONE, NONE, NONE, NONE);
    }

    void GX2::setBlendState(const BlendState& state)
    {
        GX2BlendCombineMode operationRGB;
        if (!GX2::getConstant(state.operationRGB, operationRGB))
            return;

        GX2BlendCombineMode operationA;
        if (!GX2::getConstant(state.operationA, operationA))
            return;

        GX2BlendMode sourceColor;
        if (!GX2::getConstant(state.srcFactorRGB, sourceColor))
            return;

        GX2BlendMode destColor;
        if (!GX2::getConstant(state.dstFactorRGB, destColor))
            return;

        GX2BlendMode sourceAlpha;
        if (!GX2::getConstant(state.srcFactorA, sourceAlpha))
            return;

        GX2BlendMode destAlpha;
        if (!GX2::getConstant(state.dstFactorA, destAlpha))
            return;

        GX2SetBlendControl(GX2_RENDER_TARGET_0, sourceColor, destColor, operationRGB, true, sourceAlpha,
                           destAlpha, operationA);
    }

    GX2 gx2;
} // namespace love
