#include <utilities/driver/renderer_ext.hpp>

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
    this->transform.buffer = this->Allocate(DATA, TRANSFORM_SIZE, DK_UNIFORM_BUF_ALIGNMENT);
    this->transform.state.modelView = glm::mat4(1.0f);

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
    this->transform.buffer.destroy();
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
    int width = 1280, height = 720;
    if (!this->IsHandheldMode())
        width = 1920, height = 1080;

    /* create layout for the depth buffer */
    // dk::ImageLayoutMaker { this->device }
    //     .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
    //     .setFormat(DkImageFormat_Z24S8)
    //     .setDimensions(width, height)
    //     .initialize(this->framebuffers.depthLayout);

    // /* create the depth buffer */
    // const auto poolId = MemPoolType::IMAGE;

    // const auto depthLayoutSize  = this->framebuffers.depthLayout.getSize();
    // const auto depthLayoutAlign = this->framebuffers.depthLayout.getAlignment();

    // this->framebuffers.depthMemory = this->pools.image.allocate(depthLayoutSize,
    // depthLayoutAlign);

    // const auto& depthMemBlock = this->framebuffers.depthMemory.getMemBlock();
    // auto depthMemOffset       = this->framebuffers.depthMemory.getOffset();

    // this->framebuffers.depthImage.initialize(this->framebuffers.depthLayout, depthMemBlock,
    //                                          depthMemOffset);

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

    this->framebuffers.size.width  = width;
    this->framebuffers.size.height = height;
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

    // this->framebuffers.depthMemory.destroy();
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
    // dk::ImageView depth { this->framebuffers.depthImage };

    if (texture != nullptr && texture->IsRenderTarget())
    {
        target = { texture->GetImage() };

        this->SetViewport({ 0, 0, texture->GetPixelWidth(), texture->GetPixelHeight() });
        this->framebuffers.dirty = true;
    }
    else
    {
        this->framebuffers.dirty = false;
        this->SetViewport({ 0, 0, this->framebuffers.size.width, this->framebuffers.size.height });
    }

    this->commandBuffer.bindRenderTargets(&target);
    this->commandBuffer.pushConstants(this->transform.buffer.getGpuAddr(),
                                      this->transform.buffer.getSize(), 0, TRANSFORM_SIZE,
                                      &this->transform.state);

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

    Renderer::shaderSwitches = 0;
    Renderer::drawCalls      = 0;

    this->framebuffers.slot = -1;
}

void Renderer<Console::HAC>::Register(Texture<Console::HAC>* texture, DkResHandle& handle)
{
    this->EnsureInFrame();

    auto index = -1;

    if (!this->allocator.Find(texture->GetHandle(), index))
        index = this->allocator.Allocate();

    this->descriptors.dirty = true;
    handle                  = dkMakeTextureHandle(index, index);
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

bool Renderer<Console::HAC>::Render(const Graphics<Console::HAC>::DrawCommand& command)
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
    if (!(primitive = primitiveModes.Find(command.primitveType)))
        return false;

    if (!command.handles.empty())
        this->CheckDescriptorsDirty(command.handles);

    std::memcpy(this->data + this->firstVertex, command.vertices.get(), command.size);
    this->commandBuffer.draw(*primitive, command.count, 1, this->firstVertex, 0);

    this->firstVertex += command.count;
    Renderer<>::drawCalls++;

    return true;
}

void Renderer<Console::HAC>::UseProgram(Shader<Console::HAC>::Program program)
{
    this->EnsureInFrame();

    // clang-format off
    this->commandBuffer.bindShaders(DkStageFlag_GraphicsMask, { &program.vertex->shader, &program.fragment->shader });
    this->commandBuffer.bindUniformBuffer(DkStage_Vertex, 0, this->transform.buffer.getGpuAddr(), TRANSFORM_SIZE);
    // clang-format on
}

void Renderer<Console::HAC>::SetSamplerState(Texture<Console::HAC>* texture, SamplerState& state)
{
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

    this->descriptorList[index] = samplerDescriptor;

    this->descriptors.sampler.update(this->commandBuffer, index, samplerDescriptor);

    this->descriptors.dirty = true;
}

void Renderer<Console::HAC>::SetViewport(const Rect& viewport)
{
    this->EnsureInFrame();

    DkViewport dkViewport { viewport.x, viewport.y, viewport.w, viewport.h, Z_NEAR, Z_FAR };
    this->commandBuffer.setViewports(0, { dkViewport });

    const auto ortho = glm::ortho(0.0f, (float)viewport.w, (float)viewport.h, 0.0f, Z_NEAR, Z_FAR);
    this->transform.state.projection = ortho;
}

void Renderer<Console::HAC>::SetScissor(bool enable, const Rect& scissor, bool canvasActive)
{
    this->EnsureInFrame();

    DkScissor dkScissor = { scissor.x, scissor.y, scissor.w, scissor.h };
    this->commandBuffer.setScissors(0, { dkScissor });
}

std::optional<Screen> Renderer<Console::HAC>::CheckScreen(const char* name) const
{
    return gfxScreens.Find(name);
}

SmallTrivialVector<const char*, 1> Renderer<Console::HAC>::GetScreens() const
{
    return gfxScreens.GetNames();
}
