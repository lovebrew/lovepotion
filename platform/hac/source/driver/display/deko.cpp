#include "driver/display/deko.hpp"
#include "driver/graphics/Attributes.hpp"

#include "modules/graphics/Texture.hpp"

namespace love
{
    deko3d::deko3d() :
        context {},
        transform {},
        device(dk::DeviceMaker {}.setFlags(DkDeviceFlags_DepthMinusOneToOne).create()),
        mainQueue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
        textureQueue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
        swapchain {},
        images(CMemPool(this->device, GPU_USE_FLAGS, GPU_POOL_SIZE)),
        data(CMemPool(this->device, CPU_USE_FLAGS, CPU_POOL_SIZE)),
        code(CMemPool(this->device, SHADER_USE_FLAGS, SHADER_POOL_SIZE)),
        framebufferSlot(-1)
    {}

    void deko3d::initialize()
    {
        if (this->initialized)
            return;

        this->uniform             = this->data.allocate(TRANSFORM_SIZE, DK_UNIFORM_BUF_ALIGNMENT);
        this->transform.modelView = glm::mat4(1.0f);

        this->commands.allocate(this->data, COMMAND_SIZE);
        this->commandBuffer = dk::CmdBufMaker { this->device }.create();

        this->createFramebuffers();

        this->context.rasterizer.setCullMode(DkFace_None);

        this->context.depthStencil.setDepthTestEnable(true);
        this->context.depthStencil.setDepthWriteEnable(true);
        this->context.depthStencil.setDepthCompareOp(DkCompareOp_Always);

        this->context.color.setBlendEnable(0, true);

        this->imageSet.allocate(this->data);
        this->samplerSet.allocate(this->data);

        this->ensureInFrame();

        this->imageSet.bindForImages(this->commandBuffer);
        this->samplerSet.bindForSamplers(this->commandBuffer);

        this->initialized = true;
    }

    void deko3d::deInitialize()
    {
        this->destroyFramebuffers();
        this->uniform.destroy();
    }

    void deko3d::createFramebuffers()
    {
        const auto info = getScreenInfo()[0];

        this->depthbuffer.create(info, this->device, this->images, true);

        for (size_t index = 0; index < this->targets.size(); ++index)
        {
            this->framebuffers[index].create(info, this->device, this->images, false);
            this->targets[index] = this->framebuffers[index].getImage();
        }

        this->swapchain = dk::SwapchainMaker { this->device, nwindowGetDefault(), this->targets }.create();

        this->context.viewport = Rect { 0, 0, info.width, info.height };
        this->context.scissor  = Rect { 0, 0, info.width, info.height };

        this->setViewport(this->context.viewport);
        this->setScissor(this->context.scissor);
    }

    void deko3d::destroyFramebuffers()
    {
        if (!this->swapchain)
            return;

        this->mainQueue.waitIdle();
        this->textureQueue.waitIdle();

        this->commandBuffer.clear();
        this->swapchain.destroy();

        this->context.boundFramebuffer = nullptr;
        for (auto& framebuffer : this->framebuffers)
            framebuffer.destroy();

        this->depthbuffer.destroy();
    }

    void deko3d::ensureInFrame()
    {
        if (!this->inFrame)
        {
            this->context.descriptorsDirty = false;
            this->commands.begin(this->commandBuffer);
            this->inFrame = true;
        }
    }

    void deko3d::clear(const Color& color)
    {
        if (!this->inFrame)
            return;

        this->commandBuffer.clearColor(0, DkColorMask_RGBA, color.r, color.g, color.b, color.a);
    }

    void deko3d::clearDepthStencil(int depth, uint8_t mask, double stencil)
    {
        if (!this->inFrame)
            return;

        this->commandBuffer.clearDepthStencil(true, depth, mask, stencil);
    }

    // dk::Image& deko3d::getInternalBackbuffer()
    // {
    //     this->ensureInFrame();
    //     return this->framebuffers[this->framebufferSlot].getImage();
    // }

    void deko3d::useProgram(const dk::Shader& vertex, const dk::Shader& fragment)
    {
        this->ensureInFrame();

        if (!this->uniform)
            return;

        // clang-format off
        this->commandBuffer.bindShaders(DkStageFlag_GraphicsMask, { &vertex, &fragment });
        this->commandBuffer.bindUniformBuffer(DkStage_Vertex, 0, this->uniform.getGpuAddr(), this->uniform.getSize());
        // clang-format on
    }

    void deko3d::registerTexture(TextureBase* texture, bool registering)
    {
        if (registering)
        {
            const auto index = this->textureHandles.allocate();
            texture->setHandleData(dkMakeTextureHandle(index, index));
            return;
        }

        this->textureHandles.deallocate((uint32_t)texture->getHandle());
    }

    void deko3d::bindFramebuffer(dk::Image* framebuffer)
    {
        if (!this->swapchain)
            return;

        if (this->framebufferSlot < 0)
            this->framebufferSlot = this->mainQueue.acquireImage(this->swapchain);

        if (!framebuffer)
            framebuffer = this->framebuffers[this->framebufferSlot].getImage();

        bool bindingModified = false;

        if (this->context.boundFramebuffer != framebuffer)
        {
            bindingModified                = true;
            this->context.boundFramebuffer = framebuffer;
        }

        if (bindingModified)
        {
            dk::ImageView depth { *this->depthbuffer.getImage() };
            dk::ImageView target { *framebuffer };

            this->commandBuffer.bindRenderTargets(&target, &depth);
        }
    }

    void deko3d::bindBuffer(BufferUsage usage, CMemPool::Handle& handle)
    {
        if (usage == BUFFERUSAGE_VERTEX)
            this->commandBuffer.bindVtxBuffer(0, handle.getGpuAddr(), handle.getSize());
        else if (usage == BUFFERUSAGE_INDEX)
            this->commandBuffer.bindIdxBuffer(DkIdxFormat_Uint16, handle.getGpuAddr());
    }

    void deko3d::setVertexAttributes(bool isTexture)
    {
        vertex::Attributes attributes {};
        vertex::getAttributes(isTexture, attributes);

        this->commandBuffer.bindVtxAttribState(attributes.attributeState);
        this->commandBuffer.bindVtxBufferState(attributes.bufferState);
    }

    void deko3d::bindTextureToUnit(TextureBase* texture, int unit)
    {
        if (!texture)
            return;

        const auto handle = (DkResHandle)texture->getHandle();
        this->bindTextureToUnit(handle, unit);
    }

    void deko3d::bindTextureToUnit(DkResHandle texture, int unit)
    {
        if (this->context.descriptorsDirty)
        {
            this->commandBuffer.barrier(DkBarrier_Primitives, DkInvalidateFlags_Descriptors);
            this->context.descriptorsDirty = false;
        }

        this->commandBuffer.bindTextures(DkStage_Fragment, 0, texture);
    }

    void deko3d::drawIndexed(DkPrimitive primitive, uint32_t indexCount, uint32_t indexOffset,
                             uint32_t instanceCount)
    {
        this->commandBuffer.drawIndexed(primitive, indexCount, instanceCount, indexOffset, 0, 0);
    }

    void deko3d::draw(DkPrimitive primitive, uint32_t vertexCount, uint32_t firstVertex)
    {
        this->commandBuffer.draw(primitive, vertexCount, 1, firstVertex, 0);
    }

    void deko3d::prepareDraw(GraphicsBase* graphics)
    {
        if (!this->swapchain)
            return;

        this->commandBuffer.bindRasterizerState(this->context.rasterizer);
        this->commandBuffer.bindColorState(this->context.color);
        this->commandBuffer.bindColorWriteState(this->context.colorWrite);
        this->commandBuffer.bindBlendStates(0, this->context.blend);

        if (!this->uniform)
            return;

        this->commandBuffer.pushConstants(this->uniform.getGpuAddr(), this->uniform.getSize(), 0,
                                          TRANSFORM_SIZE, &this->transform);
    }

    void deko3d::present()
    {
        if (!this->swapchain)
            return;

        if (this->inFrame)
        {
            GraphicsBase::flushBatchedDrawsGlobal();
            GraphicsBase::advanceStreamBuffersGlobal();

            this->mainQueue.submitCommands(this->commands.end(this->commandBuffer));
            this->mainQueue.presentImage(this->swapchain, this->framebufferSlot);

            this->inFrame = false;
        }

        this->framebufferSlot = -1;
    }

    void deko3d::setVertexWinding(Winding winding)
    {
        DkFrontFace face;
        if (!deko3d::getConstant(winding, face))
            return;

        this->context.rasterizer.setFrontFace(face);
    }

    void deko3d::setPointSize(float size)
    {
        this->commandBuffer.setPointSize(size);
    }

    void deko3d::setCullMode(CullMode mode)
    {
        DkFace cullMode;
        if (!deko3d::getConstant(mode, cullMode))
            return;

        this->context.rasterizer.setCullMode(cullMode);
    }

    void deko3d::setColorMask(ColorChannelMask mask)
    {
        const auto red   = (DkColorMask_R * mask.r);
        const auto green = (DkColorMask_G * mask.g);
        const auto blue  = (DkColorMask_B * mask.b);
        const auto alpha = (DkColorMask_A * mask.a);

        this->context.colorWrite.setMask(0, uint32_t(red + green + blue + alpha));
    }

    void deko3d::setBlendState(const BlendState& state)
    {
        if (this->context.blendState == state)
            return;

        DkBlendOp operationRGB;
        if (!deko3d::getConstant(state.operationRGB, operationRGB))
            return;

        DkBlendOp operationA;
        if (!deko3d::getConstant(state.operationA, operationA))
            return;

        DkBlendFactor sourceColor;
        if (!deko3d::getConstant(state.srcFactorRGB, sourceColor))
            return;

        DkBlendFactor destColor;
        if (!deko3d::getConstant(state.dstFactorRGB, destColor))
            return;

        DkBlendFactor sourceAlpha;
        if (!deko3d::getConstant(state.srcFactorA, sourceAlpha))
            return;

        DkBlendFactor destAlpha;
        if (!deko3d::getConstant(state.dstFactorA, destAlpha))
            return;

        this->context.blend.setColorBlendOp(operationRGB);
        this->context.blend.setAlphaBlendOp(operationA);

        this->context.blend.setSrcColorBlendFactor(sourceColor);
        this->context.blend.setSrcAlphaBlendFactor(sourceAlpha);

        this->context.blend.setDstColorBlendFactor(destColor);
        this->context.blend.setDstAlphaBlendFactor(destAlpha);
    }

    void deko3d::setSamplerState(TextureBase* texture, const SamplerState& state)
    {
        this->ensureInFrame();

        auto* sampler         = (dk::Sampler*)texture->getSamplerHandle();
        auto& imageDescriptor = ((Texture*)texture)->getDescriptorHandle();
        // auto& samplerDescriptor = ((Texture*)texture)->getSamplerDescriptor();

        auto index = -1;

        if (!this->textureHandles.find((uint32_t)texture->getHandle(), index))
            index = this->textureHandles.allocate();

        DkFilter magFilter;
        if (!deko3d::getConstant(state.magFilter, magFilter))
            return;

        DkFilter minFilter;
        if (!deko3d::getConstant(state.minFilter, minFilter))
            return;

        DkMipFilter mipFilter;
        if (!deko3d::getConstant(state.mipmapFilter, mipFilter))
            return;

        sampler->setFilter(minFilter, magFilter, mipFilter);

        DkWrapMode wrapU;
        if (!deko3d::getConstant(state.wrapU, wrapU))
            return;

        DkWrapMode wrapV;
        if (!deko3d::getConstant(state.wrapV, wrapV))
            return;

        DkWrapMode wrapW;
        if (!deko3d::getConstant(state.wrapW, wrapW))
            return;

        sampler->setWrapMode(wrapU, wrapV, wrapW);

        dk::SamplerDescriptor samplerDescriptor {};
        samplerDescriptor.initialize(*sampler);

        this->imageSet.update(this->commandBuffer, index, imageDescriptor);
        this->samplerSet.update(this->commandBuffer, index, samplerDescriptor);

        this->context.descriptorsDirty = true;
    }

    template<typename T>
    T dkRectFromRect(const Rect& rect)
    {
        T value {};
        value.x      = rect.x;
        value.y      = rect.y;
        value.width  = rect.w;
        value.height = rect.h;

        return value;
    }

    void deko3d::setScissor(const Rect& rect)
    {
        this->ensureInFrame();

        const auto scissor  = rect == Rect::EMPTY ? this->context.scissor : rect;
        DkScissor dkScissor = dkRectFromRect<DkScissor>(scissor);

        this->commandBuffer.setScissors(0, dkScissor);
    }

    void deko3d::setViewport(const Rect& rect)
    {
        this->ensureInFrame();

        const auto viewport   = rect == Rect::EMPTY ? this->context.viewport : rect;
        DkViewport dkViewport = dkRectFromRect<DkViewport>(viewport);

        this->commandBuffer.setViewports(0, dkViewport);

        this->transform.projection =
            glm::ortho(0.0f, (float)viewport.w, (float)viewport.h, 0.0f, -10.0f, 10.0f);
    }

    deko3d d3d;
} // namespace love
