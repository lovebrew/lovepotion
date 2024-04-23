#include "driver/display/Renderer.hpp"

namespace love
{
    Renderer::Renderer() :
        transform {},
        device(dk::DeviceMaker {}.setFlags(DkDeviceFlags_DepthMinusOneToOne).create()),
        mainQueue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
        textureQueue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
        commandBuffer(dk::CmdBufMaker { this->device }.create()),
        swapchain {},
        images(CMemPool(this->device, GPU_USE_FLAGS, GPU_POOL_SIZE)),
        data(CMemPool(this->device, CPU_USE_FLAGS, CPU_POOL_SIZE)),
        code(CMemPool(this->device, SHADER_USE_FLAGS, SHADER_POOL_SIZE)),
        framebufferSlot(-1)
    {}

    void Renderer::initialize()
    {
        if (this->initialized)
            return;

        this->unformBuffer        = this->data.allocate(TRANSFORM_SIZE, DK_UNIFORM_BUF_ALIGNMENT);
        this->transform.modelView = glm::mat4(1.0f);

        this->commands.allocate(this->data, COMMAND_SIZE);
        this->createFramebuffers();

        this->initialized = true;
    }

    Renderer::~Renderer()
    {
        this->destroyFramebuffers();
        this->unformBuffer.destroy();
    }

    void Renderer::createFramebuffers()
    {
        const auto& info = getScreenInfo()[0];

        this->depthbuffer.create(info, this->device, this->images, true);

        for (size_t index = 0; index < this->targets.size(); ++index)
        {
            this->framebuffers[index].create(info, this->device, this->images, false);
            this->targets[index] = &this->framebuffers[index].getImage();
        }

        this->swapchain =
            dk::SwapchainMaker { this->device, nwindowGetDefault(), this->targets }.create();

        this->context.viewport = Rect { 0, 0, info.width, info.height };
        this->context.scissor  = Rect { 0, 0, info.width, info.height };

        this->setViewport(this->context.viewport);
        this->setScissor(this->context.scissor);
    }

    void Renderer::destroyFramebuffers()
    {
        if (!this->swapchain)
            return;

        this->mainQueue.waitIdle();
        this->textureQueue.waitIdle();

        this->commandBuffer.clear();
        this->swapchain.destroy();

        for (auto& framebuffer : this->framebuffers)
            framebuffer.destroy();

        this->depthbuffer.destroy();
    }

    void Renderer::ensureInFrame()
    {
        if (!this->inFrame)
        {
            this->commands.begin(this->commandBuffer);
            this->inFrame = true;
        }
    }

    void Renderer::clear(const Color& color)
    {
        this->commandBuffer.clearColor(0, DkColorMask_RGBA, color.r, color.g, color.b, color.a);
    }

    void Renderer::clearDepthStencil(int depth, uint8_t mask, double stencil)
    {
        this->commandBuffer.clearDepthStencil(true, depth, mask, stencil);
    }

    void Renderer::bindFramebuffer()
    {
        if (!this->swapchain)
            return;

        this->ensureInFrame();

        if (this->framebufferSlot < 0)
            this->framebufferSlot = this->mainQueue.acquireImage(this->swapchain);

        dk::ImageView target { this->framebuffers[this->framebufferSlot].getImage() };
        dk::ImageView depth { this->depthbuffer.getImage() };

        this->commandBuffer.bindRenderTargets(&target, &depth);
    }

    void Renderer::present()
    {
        if (!this->swapchain)
            return;

        if (this->inFrame)
        {
            this->mainQueue.submitCommands(this->commands.end(this->commandBuffer));
            this->mainQueue.presentImage(this->swapchain, this->framebufferSlot);

            this->inFrame = false;
        }

        this->framebufferSlot = -1;
    }

    void Renderer::setVertexWinding(Winding winding)
    {
        DkFrontFace face;
        if (!Renderer::getConstant(winding, face))
            return;

        this->context.rasterizer.setFrontFace(face);
    }

    void Renderer::setCullMode(CullMode mode)
    {
        DkFace cullMode;
        if (!Renderer::getConstant(mode, cullMode))
            return;

        this->context.rasterizer.setCullMode(cullMode);
    }

    void Renderer::setColorMask(ColorChannelMask mask)
    {
        const auto red   = (DkColorMask_R * mask.r);
        const auto green = (DkColorMask_G * mask.g);
        const auto blue  = (DkColorMask_B * mask.b);
        const auto alpha = (DkColorMask_A * mask.a);

        this->context.colorWrite.setMask(0, (red + green + blue + alpha));
    }

    void Renderer::setBlendState(const BlendState& state)
    {
        if (this->context.blendState == state)
            return;

        DkBlendOp operationRGB;
        if (!Renderer::getConstant(state.operationRGB, operationRGB))
            return;

        DkBlendOp operationA;
        if (!Renderer::getConstant(state.operationA, operationA))
            return;

        DkBlendFactor sourceColor;
        if (!Renderer::getConstant(state.srcFactorRGB, sourceColor))
            return;

        DkBlendFactor destColor;
        if (!Renderer::getConstant(state.dstFactorRGB, destColor))
            return;

        DkBlendFactor sourceAlpha;
        if (!Renderer::getConstant(state.srcFactorA, sourceAlpha))
            return;

        DkBlendFactor destAlpha;
        if (!Renderer::getConstant(state.dstFactorA, destAlpha))
            return;

        this->context.blend.setColorBlendOp(operationRGB);
        this->context.blend.setAlphaBlendOp(operationA);

        // Blend factors
        this->context.blend.setSrcColorBlendFactor(sourceColor);
        this->context.blend.setSrcAlphaBlendFactor(sourceAlpha);

        this->context.blend.setDstColorBlendFactor(destColor);
        this->context.blend.setDstAlphaBlendFactor(destAlpha);
    }

    static DkScissor dkScissorFromRect(const Rect& rect)
    {
        DkScissor scissor {};

        scissor.x      = (uint32_t)rect.x;
        scissor.y      = (uint32_t)rect.y;
        scissor.width  = (uint32_t)rect.w;
        scissor.height = (uint32_t)rect.h;

        return scissor;
    }

    void Renderer::setScissor(const Rect& scissor)
    {
        this->ensureInFrame();
        DkScissor _scissor {};

        if (scissor == Rect::EMPTY)
            _scissor = dkScissorFromRect(this->context.scissor);
        else
            _scissor = dkScissorFromRect(scissor);

        this->commandBuffer.setScissors(0, { _scissor });
    }

    static DkViewport dkViewportFromRect(const Rect& rect)
    {
        DkViewport viewport {};

        viewport.x      = (float)rect.x;
        viewport.y      = (float)rect.y;
        viewport.width  = (float)rect.w;
        viewport.height = (float)rect.h;
        viewport.near   = Framebuffer::Z_NEAR;
        viewport.far    = Framebuffer::Z_FAR;

        return viewport;
    }

    void Renderer::setViewport(const Rect& viewport)
    {
        this->ensureInFrame();
        DkViewport _viewport {};

        if (viewport == Rect::EMPTY)
            _viewport = dkViewportFromRect(this->context.viewport);
        else
            _viewport = dkViewportFromRect(viewport);

        this->commandBuffer.setViewports(0, { _viewport });
    }
} // namespace love
