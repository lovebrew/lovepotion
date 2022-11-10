#include <utilities/driver/renderer_ext.hpp>

using namespace love;

int Renderer<>::shaderSwitches = 0;
int Renderer<>::drawCalls      = 0;

Renderer<Console::HAC>::Renderer() :
    firstVertex(0),
    data(nullptr),
    device { dk::DeviceMaker {}.setFlags(DkDeviceFlags_DepthMinusOneToOne).create() },
    mainQueue { dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create() },
    textureQueue { dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create() },
    commandBuffer {},
    swapChain {},
    pools { .image = CMemPool(this->device, GPU_USE_FLAGS, GPU_POOL_SIZE),
            .data  = CMemPool(this->device, CPU_USE_FLAGS, CPU_POOL_SIZE),
            .code  = CMemPool(this->device, SHADER_USE_FLAGS, SHADER_POOL_SIZE) },
    state {},
    framebuffers {},
    depthBuffer {},
    transform {},
    descriptors {},
    filter {}
{
    /* create our Transform information */
    this->transform.buffer = this->Allocate(DATA, TRANSFORM_SIZE, DK_UNIFORM_BUF_ALIGNMENT);
    this->transform.state.modelView = glm::mat4(1.0f);

    /* allocate descriptors */
    this->descriptors.image.allocate(this->pools.data);
    this->descriptors.sampler.allocate(this->pools.data);

    /* set up the device depth state */
    this->state.depthStencil.setDepthTestEnable(true);
    this->state.depthStencil.setDepthWriteEnable(true);
    this->state.depthStencil.setDepthCompareOp(DkCompareOp_Always);

    /* set up the device color state */
    this->state.color.setBlendEnable(0, true);

    /* stup up device texture filter */
    this->filter.sampler.setFilter(DkFilter_Linear, DkFilter_Linear);
    this->filter.sampler.setWrapMode(DkWrapMode_Clamp, DkWrapMode_Clamp);
    this->filter.descriptor.initialize(this->filter.sampler);

    this->commandBuffer = dk::CmdBufMaker { this->device }.create();
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

    /* initialize depth buffer layout */
    dk::ImageLayoutMaker { this->device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_Z24S8)
        .setDimensions(width, height)
        .initialize(this->depthBuffer.layout);

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
        const auto offset    = memory.getOffset();

        this->framebuffers.images[index].initialize(this->framebuffers.layout, memBlock, offset);
        this->rendertargets[index] = &this->framebuffers.images[index];
    }

    this->swapChain =
        dk::SwapchainMaker { this->device, nwindowGetDefault(), this->rendertargets }.create();
}

void Renderer<Console::HAC>::DestroyFramebuffers()
{
    if (!this->swapChain)
        return;

    this->mainQueue.waitIdle();
    this->textureQueue.waitIdle();

    this->commandBuffer.clear();

    this->swapChain.destroy();

    for (auto& framebuffer : this->framebuffers.memory)
        framebuffer.destroy();

    this->depthBuffer.memory.destroy();
}

void Renderer<Console::HAC>::EnsureInFrame()
{
    if (this->inFrame)
        return;

    this->commands.begin(this->commandBuffer);
    this->inFrame = true;
}

void Renderer<Console::HAC>::Clear(const Color& color)
{
    this->EnsureInFrame();
    this->commandBuffer.clearColor(0, DkColorMask_RGBA, color.r, color.g, color.b, color.a);
}

void Renderer<Console::HAC>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{}

void Renderer<Console::HAC>::SetBlendColor(const Color& color)
{
    this->EnsureInFrame();
    this->commandBuffer.setBlendConst(color.r, color.g, color.b, color.a);
}

void Renderer<Console::HAC>::BindFramebuffer(/* Canvas* canvas */)
{
    if (!this->swapChain)
        return;

    this->EnsureInFrame();

    if (this->framebuffers.slot < 0)
        this->framebuffers.slot = this->mainQueue.acquireImage(this->swapChain);

    if (this->framebuffers.dirty)
        this->commandBuffer.barrier(DkBarrier_Fragments, 0);

    dk::ImageView target { this->framebuffers.images[this->framebuffers.slot] };

    this->commandBuffer.bindRenderTargets(&target);
    this->commandBuffer.pushConstants(this->transform.buffer.getGpuAddr(),
                                      this->transform.buffer.getSize(), 0, TRANSFORM_SIZE,
                                      &this->transform.state);

    /* begin vertex ring */
    const auto& data = this->vertices.begin();
    this->data       = (vertex::Vertex*)data.first;

    this->commandBuffer.bindRasterizerState(this->state.rasterizer);
    this->commandBuffer.bindColorState(this->state.color);
    this->commandBuffer.bindColorWriteState(this->state.colorWrite);
    this->commandBuffer.bindBlendStates(0, this->state.blend);

    this->commandBuffer.bindVtxBuffer(0, data.second, this->vertices.getSize());
}

void Renderer<Console::HAC>::Present()
{
    if (!this->swapChain)
        return;

    if (inFrame)
    {
        this->vertices.end();

        this->mainQueue.submitCommands(this->commands.end(this->commandBuffer));
        this->mainQueue.presentImage(this->swapChain, this->framebuffers.slot);

        this->firstVertex       = 0;
        this->descriptors.dirty = false;

        this->inFrame = false;
    }

    Renderer<>::shaderSwitches = 0;
    Renderer<>::drawCalls      = 0;

    this->framebuffers.slot = -1;
}

void Renderer<Console::HAC>::SetViewport(const Rect& viewport)
{
    this->EnsureInFrame();

    DkViewport dkViewport { viewport.x, viewport.y, viewport.w, viewport.h, Z_NEAR, Z_FAR };
    this->commandBuffer.setViewports(0, dkViewport);

    const auto ortho = glm::ortho(0.0f, (float)viewport.w, (float)viewport.h, 0.0f, Z_NEAR, Z_FAR);
    this->transform.state.projection = ortho;
}

void Renderer<Console::HAC>::SetScissor(bool enable, const Rect& scissor, bool canvasActive)
{
    this->EnsureInFrame();

    DkScissor dkScissor = { scissor.x, scissor.y, scissor.w, scissor.h };
    this->commandBuffer.setScissors(0, dkScissor);
}
