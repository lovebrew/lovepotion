#include "common/screen.hpp"

#include "driver/display/Renderer.hpp"

namespace love
{
    Renderer::Renderer() : context {}
    {
        this->targets.reserve(3);
    }

    void Renderer::initialize()
    {
        if (this->initialized)
            return;

        gfxInitDefault();

        if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
            throw love::Exception("Failed to initialize citro3d.");

        C3D_CullFace(GPU_CULL_NONE);
        C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

        Mtx_Identity(&this->context.modelView);
        Mtx_Identity(&this->context.projection);

        this->set3DMode(true);

        this->initialized = true;
    }

    Renderer::~Renderer()
    {
        this->destroyFramebuffers();

        C3D_Fini();
        gfxExit();
    }

    void Renderer::createFramebuffers()
    {
        const auto& info = getScreenInfo();

        for (size_t index = 0; index < info.size(); ++index)
        {
            Framebuffer target {};
            target.create(info[index]);

            this->targets.push_back(std::move(target));
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
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            this->inFrame = true;
        }
    }

    void Renderer::clear(const Color& color)
    {
        C3D_RenderTargetClear(this->context.target.get(), C3D_CLEAR_ALL, color.abgr(), 0);
    }

    void Renderer::clearDepthStencil(int depth, uint8_t mask, double stencil)
    {
        /// ???
    }

    void Renderer::bindFramebuffer()
    {
        this->ensureInFrame();

        this->context.target = this->targets[currentScreen];
        auto viewport        = this->context.target.getViewport();

        C3D_FrameDrawOn(this->context.target.get());
        this->setViewport(viewport, this->context.target.get()->linked);
    }

    void Renderer::present()
    {
        if (this->inFrame)
        {
            C3D_FrameEnd(0);
            this->inFrame = false;
        }
    }

    void Renderer::setViewport(const Rect& viewport, bool tilt)
    {
        if (this->context.viewport == viewport)
            return;

        this->context.viewport = viewport;

        if (viewport.h == GSP_SCREEN_WIDTH && tilt)
        {
            if (viewport.w == GSP_SCREEN_HEIGHT_TOP || viewport.w == GSP_SCREEN_HEIGHT_TOP_2X)
            {
                Mtx_Copy(&this->context.projection, &this->targets[0].getProjection());
                this->context.dirtyProjection = true;
                return;
            }
            else if (viewport.w == GSP_SCREEN_HEIGHT_BOTTOM)
            {
                const auto index = gfxIs3D() ? 2 : 1;

                Mtx_Copy(&this->context.projection, &this->targets[index].getProjection());
                this->context.dirtyProjection = true;
                return;
            }
        }

        auto* ortho = tilt ? Mtx_OrthoTilt : Mtx_Ortho;
        ortho(&this->context.projection, 0.0f, viewport.w, viewport.h, 0.0f, Framebuffer::Z_NEAR,
              Framebuffer::Z_FAR, true);

        this->context.dirtyProjection = true;
        C3D_SetViewport(0, 0, (uint32_t)viewport.w, (uint32_t)viewport.h);
    }

    void Renderer::setScissor(const Rect& scissor)
    {
        this->context.target.setScissor(scissor);
    }

    void Renderer::setCullMode(CullMode mode)
    {
        GPU_CULLMODE cullMode;
        if (!Renderer::getConstant(mode, cullMode))
            return;

        if (this->context.cullMode == mode)
            return;

        C3D_CullFace(cullMode);
        this->context.cullMode = mode;
    }

    void Renderer::setVertexWinding(Winding winding)
    {}

    void Renderer::setColorMask(ColorChannelMask mask)
    {
        uint8_t writeMask = GPU_WRITE_DEPTH;
        writeMask |= mask.get();

        if (this->context.colorMask == mask)
            return;

        this->context.colorMask = mask;
        C3D_DepthTest(true, GPU_GEQUAL, (GPU_WRITEMASK)writeMask);
    }

    void Renderer::setBlendState(const BlendState& state)
    {
        if (this->context.blendState == state)
            return;

        GPU_BLENDEQUATION operationRGB;
        if (!Renderer::getConstant(state.operationRGB, operationRGB))
            return;

        GPU_BLENDEQUATION operationA;
        if (!Renderer::getConstant(state.operationA, operationA))
            return;

        GPU_BLENDFACTOR sourceColor;
        if (!Renderer::getConstant(state.srcFactorRGB, sourceColor))
            return;

        GPU_BLENDFACTOR destColor;
        if (!Renderer::getConstant(state.dstFactorRGB, destColor))
            return;

        GPU_BLENDFACTOR sourceAlpha;
        if (!Renderer::getConstant(state.srcFactorA, sourceAlpha))
            return;

        GPU_BLENDFACTOR destAlpha;
        if (!Renderer::getConstant(state.dstFactorA, destAlpha))
            return;

        C3D_AlphaBlend(operationRGB, operationA, sourceColor, destColor, sourceAlpha, destAlpha);
        this->context.blendState = state;
    }
} // namespace love
