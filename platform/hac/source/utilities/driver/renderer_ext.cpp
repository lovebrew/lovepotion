#include <utilities/driver/renderer_ext.hpp>

#include <modules/graphics_ext.hpp>

#include <objects/shader_ext.hpp>
#include <objects/texture_ext.hpp>

using namespace love;

Renderer<Console::HAC>::Renderer() :
    transform {},
    firstVertex(0),
    data(nullptr),
    device(dk::DeviceMaker {}.setFlags(DkDeviceFlags_DepthMinusOneToOne).create()),
    mainQueue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
    textureQueue(dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create()),
    commandBuffer {},
    swapchain {},
    pools { .image = CMemPool(this->device, GPU_USE_FLAGS, GPU_POOL_SIZE),
            .data  = CMemPool(this->device, CPU_USE_FLAGS, CPU_POOL_SIZE),
            .code  = CMemPool(this->device, SHADER_USE_FLAGS, SHADER_POOL_SIZE) },
    state {},
    framebuffers {},
    descriptors {}
{
    /* create our Transform information */
    this->uniformBuffer       = this->pools.data.allocate(TRANSFORM_SIZE, DK_UNIFORM_BUF_ALIGNMENT);
    this->transform.modelView = glm::mat4(1.0f);

    /* allocate descriptors */
    this->descriptors.image.allocate(this->pools.data);
    this->descriptors.sampler.allocate(this->pools.data);

    /* allocate our rings */
    this->commands.allocate(this->pools.data, COMMAND_SIZE);
    this->vertices.allocate(this->pools.data, VERTEX_COMMAND_SIZE / 2);

    /* set up the device depth state */
    this->state.depthStencil.setDepthTestEnable(true);
    this->state.depthStencil.setDepthWriteEnable(true);
    this->state.depthStencil.setDepthCompareOp(DkCompareOp_Always);

    this->state.rasterizer.setCullMode(DkFace_None);
    this->state.rasterizer.setFrontFace(DkFrontFace_CCW);

    /* set up the device color state */
    this->state.color.setBlendEnable(0, true);
    this->commandBuffer = dk::CmdBufMaker { this->device }.create();

    this->EnsureInFrame();

    this->descriptors.image.bindForImages(this->commandBuffer);
    this->descriptors.sampler.bindForSamplers(this->commandBuffer);
}

Renderer<Console::HAC>::~Renderer()
{
    this->DestroyFramebuffers();
    this->uniformBuffer.destroy();
}

CMemPool& Renderer<Console::HAC>::GetMemPool(MemPoolType type)
{
    switch (type)
    {
        case MemPoolType::DATA:
            return this->pools.data;
        case MemPoolType::CODE:
            return this->pools.code;
        case MemPoolType::IMAGE:
        default:
            return this->pools.image;
    }
}

dk::Queue Renderer<Console::HAC>::GetQueue(QueueType type)
{
    switch (type)
    {
        case QueueType::QUEUE_IMAGES:
            return this->textureQueue;
        case QueueType::QUEUE_MAIN:
        default:
            return this->mainQueue;
    }
}

CMemPool::Handle Renderer<Console::HAC>::Allocate(MemPoolType type, size_t size, uint32_t alignment)
{
    auto& pool = this->GetMemPool(type);
    return pool.allocate(size, alignment);
}

bool Renderer<Console::HAC>::IsHandheldMode() const
{
    return appletGetOperationMode() == AppletOperationMode_Handheld;
}

void Renderer<Console::HAC>::CreateFramebuffers()
{
    int width  = 1280;
    int height = 720;

    if (!this->IsHandheldMode())
    {
        width  = 1920;
        height = 1080;
    }

    /* create layout for the depth buffer */
    dk::ImageLayoutMaker { this->device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_Z24S8)
        .setDimensions(width, height)
        .initialize(this->framebuffers.depthLayout);

    // /* create the depth buffer */
    // const auto poolId = MemPoolType::IMAGE;

    const auto depthLayoutSize  = this->framebuffers.depthLayout.getSize();
    const auto depthLayoutAlign = this->framebuffers.depthLayout.getAlignment();

    this->framebuffers.depthMemory = this->Allocate(IMAGE, depthLayoutSize, depthLayoutAlign);

    const auto& depthMemBlock = this->framebuffers.depthMemory.getMemBlock();
    auto depthMemOffset       = this->framebuffers.depthMemory.getOffset();

    this->framebuffers.depthImage.initialize(this->framebuffers.depthLayout, depthMemBlock,
                                             depthMemOffset);

    /* initialize framebuffer layout */
    dk::ImageLayoutMaker { this->device }
        .setFlags(RENDERTARGET_USE_FLAGS)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(width, height)
        .initialize(this->framebuffers.layout);

    const auto size      = this->framebuffers.layout.getSize();
    const auto alignment = this->framebuffers.layout.getAlignment();

    for (size_t index = 0; index < MAX_RENDERTARGETS; index++)
    {
        this->framebuffers.memory[index] = this->Allocate(IMAGE, size, alignment);

        const auto memory = this->framebuffers.memory[index];

        const auto& memBlock = memory.getMemBlock();
        auto offset          = memory.getOffset();

        this->framebuffers.images[index].initialize(this->framebuffers.layout, memBlock, offset);
        this->rendertargets[index] = &this->framebuffers.images[index];
    }

    this->swapchain =
        dk::SwapchainMaker { this->device, nwindowGetDefault(), this->rendertargets }.create();

    this->viewport = { 0, 0, width, height };
    love::SetScreenSize(width, height);
}

void Renderer<Console::HAC>::DestroyFramebuffers()
{
    if (!this->swapchain)
        return;

    this->mainQueue.waitIdle();
    this->textureQueue.waitIdle();

    this->commandBuffer.clear();

    this->swapchain.destroy();

    for (auto& framebuffer : this->framebuffers.memory)
        framebuffer.destroy();

    this->framebuffers.depthMemory.destroy();
}

void Renderer<Console::HAC>::EnsureInFrame()
{
    if (!this->inFrame)
    {
        this->firstVertex       = 0;
        this->descriptors.dirty = false;

        this->commands.begin(this->commandBuffer);
        this->inFrame = true;
    }
}

void Renderer<Console::HAC>::Clear(const Color& color)
{
    this->EnsureInFrame();
    this->commandBuffer.clearColor(0, DkColorMask_RGBA, color.r, color.g, color.b, color.a);
}

void Renderer<Console::HAC>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{
    this->EnsureInFrame();
    this->commandBuffer.clearDepthStencil(true, depth, mask, stencil);
}

void Renderer<Console::HAC>::SetBlendColor(const Color& color)
{
    this->EnsureInFrame();
    this->commandBuffer.setBlendConst(color.r, color.g, color.b, color.a);
}

void Renderer<Console::HAC>::BindFramebuffer(Texture<Console::HAC>* texture)
{
    if (!this->swapchain)
        return;

    this->EnsureInFrame();

    if (this->framebuffers.slot < 0)
        this->framebuffers.slot = this->mainQueue.acquireImage(this->swapchain);

    if (this->framebuffers.dirty)
        this->commandBuffer.barrier(DkBarrier_Fragments, 0);

    dk::ImageView target { this->framebuffers.images[this->framebuffers.slot] };
    dk::ImageView depth { this->framebuffers.depthImage };

    if (texture != nullptr && texture->IsRenderTarget())
    {
        target = { texture->GetImage() };

        this->SetViewport({ 0, 0, texture->GetPixelWidth(), texture->GetPixelHeight() });
        this->framebuffers.dirty = true;
    }
    else
    {
        if (this->framebuffers.dirty)
        {
            this->SetViewport(this->viewport);
            this->framebuffers.dirty = false;
        }
    }

    this->commandBuffer.bindRenderTargets(&target, &depth);

    this->commandBuffer.pushConstants(this->uniformBuffer.getGpuAddr(),
                                      this->uniformBuffer.getSize(), 0, TRANSFORM_SIZE,
                                      &this->transform);

    /* begin vertex ring */
    auto ring  = this->vertices.begin();
    this->data = (vertex::Vertex*)ring.first;

    this->commandBuffer.bindRasterizerState(this->state.rasterizer);
    // this->commandBuffer.bindDepthStencilState(this->state.depthStencil);
    this->commandBuffer.bindColorState(this->state.color);
    this->commandBuffer.bindColorWriteState(this->state.colorWrite);
    this->commandBuffer.bindBlendStates(0, this->state.blend);

    this->commandBuffer.bindVtxBuffer(0, ring.second, this->vertices.getSize());
}

void Renderer<Console::HAC>::Present()
{
    if (!this->swapchain)
        return;

    if (this->inFrame)
    {
        this->vertices.end();

        this->mainQueue.submitCommands(this->commands.end(this->commandBuffer));
        this->mainQueue.presentImage(this->swapchain, this->framebuffers.slot);

        this->inFrame = false;
    }

    this->framebuffers.slot = -1;
}

void Renderer<Console::HAC>::Register(Texture<Console::HAC>* texture, DkResHandle& handle)
{
    this->EnsureInFrame();

    const auto index = this->allocator.Allocate();

    handle = dkMakeTextureHandle(index, index);
}

void Renderer<Console::HAC>::UnRegister(Texture<Console::HAC>* texture)
{
    const auto handle = texture->GetHandle();
    this->allocator.DeAllocate(handle);
}

void Renderer<Console::HAC>::CheckDescriptorsDirty(const std::vector<DkResHandle>& handles)
{
    if (this->descriptors.dirty)
    {
        this->commandBuffer.barrier(DkBarrier_Primitives, DkInvalidateFlags_Descriptors);
        this->descriptors.dirty = false;
    }

    this->commandBuffer.bindTextures(DkStage_Fragment, 0, handles);
}

void Renderer<Console::HAC>::SetAttributes(const vertex::attributes::Attribs& attributes)
{
    this->commandBuffer.bindVtxAttribState(attributes.attributeState);
    this->commandBuffer.bindVtxBufferState(attributes.bufferState);
}

bool Renderer<Console::HAC>::Render(const DrawCommand<Console::HAC>& command)
{
    if (command.count > (this->vertices.getSize() - this->firstVertex))
        return false;

    {
        Shader<Console::HAC>::defaults[command.shader]->Attach();

        vertex::attributes::Attribs attributes {};
        vertex::attributes::GetAttributes(command.format, attributes);

        this->SetAttributes(attributes);
    }

    std::optional<DkPrimitive> primitive;
    if (!(primitive = primitiveModes.Find(command.type)))
        return false;

    if (!command.handles.empty())
    {
        std::vector<DkResHandle> handles {};
        for (size_t index = 0; index < command.handles.size(); index++)
            handles.push_back(command.handles[index]->GetHandle());

        this->CheckDescriptorsDirty(handles);
    }

    std::memcpy(this->data + this->firstVertex, command.vertices.get(), command.size);
    this->commandBuffer.draw(*primitive, command.count, 1, this->firstVertex, 0);

    this->firstVertex += command.count;
    ++drawCalls;

    return true;
}

void Renderer<Console::HAC>::UseProgram(Shader<Console::HAC>::Program program)
{
    this->EnsureInFrame();

    // clang-format off
    this->commandBuffer.bindShaders(DkStageFlag_GraphicsMask, { &program.vertex->shader, &program.fragment->shader });
    this->commandBuffer.bindUniformBuffer(DkStage_Vertex, 0, this->uniformBuffer.getGpuAddr(), this->uniformBuffer.getSize());
    // clang-format on
}

void Renderer<Console::HAC>::SetBlendMode(const RenderState::BlendState& state)
{
    std::optional<DkBlendOp> opRGB;
    if (!(opRGB = Renderer::blendEquations.Find(state.operationRGB)))
        return;

    std::optional<DkBlendOp> opAlpha;
    if (!(opAlpha = Renderer::blendEquations.Find(state.operationA)))
        return;

    std::optional<DkBlendFactor> srcColor;
    if (!(srcColor = Renderer::blendFactors.Find(state.srcFactorRGB)))
        return;

    std::optional<DkBlendFactor> dstColor;
    if (!(dstColor = Renderer::blendFactors.Find(state.dstFactorRGB)))
        return;

    std::optional<DkBlendFactor> srcAlpha;
    if (!(srcAlpha = Renderer::blendFactors.Find(state.srcFactorA)))
        return;

    std::optional<DkBlendFactor> dstAlpha;
    if (!(dstAlpha = Renderer::blendFactors.Find(state.dstFactorA)))
        return;

    this->state.blend.setColorBlendOp(*opRGB);
    this->state.blend.setAlphaBlendOp(*opAlpha);

    // Blend factors
    this->state.blend.setSrcColorBlendFactor(*srcColor);
    this->state.blend.setSrcAlphaBlendFactor(*srcAlpha);

    this->state.blend.setDstColorBlendFactor(*dstColor);
    this->state.blend.setDstAlphaBlendFactor(*dstAlpha);
}

void Renderer<Console::HAC>::SetColorMask(const RenderState::ColorMask& mask)
{
    auto writeMask = uint32_t(DkColorMask_R * mask.r + DkColorMask_G * mask.g +
                              DkColorMask_B * mask.b + DkColorMask_A * mask.a);

    this->state.colorWrite.setMask(0, writeMask);
}

void Renderer<Console::HAC>::SetSamplerState(Texture<Console::HAC>* texture, SamplerState& state)
{
    this->EnsureInFrame();

    auto index = -1;

    if (!this->allocator.Find(texture->GetHandle(), index))
        index = this->allocator.Allocate();

    auto& descriptor = texture->GetDescriptor();
    auto& sampler    = texture->GetSampler();

    /* filter modes */

    std::optional<DkFilter> mag;
    if (!(mag = Renderer::filterModes.Find(state.magFilter)))
        return;

    std::optional<DkFilter> min;
    if (!(min = Renderer::filterModes.Find(state.minFilter)))
        return;

    std::optional<DkMipFilter> mipmap;
    if (!(mipmap = Renderer::mipmapFilterModes.Find(state.mipmapFilter)))
        return;

    sampler.setFilter(*min, *mag, *mipmap);

    /* wrapping modes */

    std::optional<DkWrapMode> wrapU;
    if (!(wrapU = Renderer::wrapModes.Find(state.wrapU)))
        return;

    std::optional<DkWrapMode> wrapV;
    if (!(wrapV = Renderer::wrapModes.Find(state.wrapV)))
        return;

    std::optional<DkWrapMode> wrapW;
    if (!(wrapW = Renderer::wrapModes.Find(state.wrapW)))
        return;

    sampler.setWrapMode(*wrapU, *wrapV, *wrapW);

    this->descriptors.image.update(this->commandBuffer, index, descriptor);

    dk::SamplerDescriptor samplerDescriptor {};
    samplerDescriptor.initialize(sampler);

    this->descriptors.sampler.update(this->commandBuffer, index, samplerDescriptor);

    this->descriptors.dirty = true;
}

void Renderer<Console::HAC>::SetStencil(RenderState::CompareMode mode, int value)
{
    bool enabled = (mode == RenderState::COMPARE_ALWAYS) ? false : true;

    std::optional<DkCompareOp> operation;
    if (!(operation = Renderer::compareModes.Find(mode)))
        return;

    this->state.depthStencil.setDepthTestEnable(enabled);
    this->state.depthStencil.setDepthCompareOp(*operation);
}

void Renderer<Console::HAC>::SetMeshCullMode(vertex::CullMode mode)
{
    std::optional<DkFace> cullFace;
    if (!(cullFace = Renderer::cullModes.Find(mode)))
        return;

    this->state.rasterizer.setCullMode(*cullFace);
}

void Renderer<Console::HAC>::SetVertexWinding(vertex::Winding winding)
{
    std::optional<DkFrontFace> frontFace;
    if (!(frontFace = Renderer::windingModes.Find(winding)))
        return;

    this->state.rasterizer.setFrontFace(*frontFace);
}

void Renderer<Console::HAC>::SetLineWidth(float width)
{
    this->EnsureInFrame();
    this->commandBuffer.setLineWidth(width);
}

void Renderer<Console::HAC>::SetLineStyle(RenderState::LineStyle style)
{
    // bool smooth = (style == RenderState::LINE_SMOOTH);
    // this->state.rasterizer.setPolygonSmoothEnable(smooth);
}

void Renderer<Console::HAC>::SetPointSize(float size)
{
    this->EnsureInFrame();
    this->commandBuffer.setPointSize(size);
}

static void dkScissorFromRect(const Rect& scissor, DkScissor& out)
{
    out.x      = (uint32_t)scissor.x;
    out.y      = (uint32_t)scissor.y;
    out.width  = (uint32_t)scissor.w;
    out.height = (uint32_t)scissor.h;
}

void Renderer<Console::HAC>::SetScissor(const Rect& scissor, bool canvasActive)
{
    this->EnsureInFrame();
    DkScissor dkScissor {};

    if (scissor == Rect::EMPTY)
        dkScissorFromRect(this->viewport, dkScissor);
    else
        dkScissorFromRect(scissor, dkScissor);

    this->commandBuffer.setScissors(0, { dkScissor });
}

static void dkViewportFromRect(const Rect& viewport, DkViewport& out)
{
    out.x      = (float)viewport.x;
    out.y      = (float)viewport.y;
    out.width  = (float)viewport.w;
    out.height = (float)viewport.h;

    out.near = Renderer<>::Z_NEAR;
    out.far  = Renderer<>::Z_FAR;
}

void Renderer<Console::HAC>::SetViewport(const Rect& viewport)
{
    this->EnsureInFrame();

    DkViewport dkViewport {};
    dkViewportFromRect(viewport, dkViewport);

    this->commandBuffer.setViewports(0, { dkViewport });

    const auto ortho = glm::ortho(0.0f, (float)viewport.w, (float)viewport.h, 0.0f, Z_NEAR, Z_FAR);
    this->transform.projection = ortho;
}
