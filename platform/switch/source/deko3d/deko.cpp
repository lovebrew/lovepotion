#include "common/runtime.h"
#include "deko3d/deko.h"

#include "deko3d/vertex.h"

deko3d::deko3d()
{
    /*
    ** Create GPU device
    ** default origin is top left
    ** Unique -- destroys automatically
    */
    this->device = dk::DeviceMaker{}
                        .create();

    /*
    ** Render Queue
    ** Unique -- destroys automatically
    */
    this->queue = dk::QueueMaker{this->device}
                        .setFlags(DkQueueFlags_Graphics)
                        .create();

    // Create GPU & CPU Memory Pools
    auto gpuFlags = (DkMemBlockFlags_GpuCached   | DkMemBlockFlags_Image    );
    auto cpuFlags = (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached);

    this->pool.images.emplace(this->device, gpuFlags, 64 * 1024 * 1024);
    this->pool.data.emplace(this->device,   cpuFlags, 1  * 1024 * 1024);

    // Used for Shader code
    auto shaderFlags = (DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code);
    this->pool.code.emplace(this->device, shaderFlags, 128 * 1024);

    // Create the dynamic command buffer
    this->cmdBuf = dk::CmdBufMaker{this->device}
                        .create();

    this->cmdRing.allocate(*this->pool.data, COMMAND_SIZE);
    this->vtxRing.allocate(*this->pool.data, VERTEX_COMMAND_SIZE / 2);

    this->state.color.setBlendEnable(0, true);

    this->CreateResources();

    // Initialize the projection matrix
    this->SetViewport({0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT});

    this->transformState.projMtx = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -10.0f, 10.0f);
    this->transformState.mdlvMtx = glm::mat4(1.0f);
}

deko3d::~deko3d()
{
    // Return early if we have nothing to destroy
    if (!this->swapchain)
        return;

    // Make sure the queue is idle before destroying anything
    this->queue.waitIdle();

    // Clear the static cmdbuf, destroying the static cmdlists in the process
    this->cmdBuf.clear();

    // Destroy the swapchain
    this->swapchain.destroy();

    this->transformUniformBuffer.destroy();

    // Destroy the framebuffers
    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i ++)
        this->framebuffers.memory[i].destroy();
}


void deko3d::CreateResources()
{
    // TODO: depth buffer

    // Create a layout for the framebuffers
    auto layoutFlags = (DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression);

    dk::ImageLayoutMaker {this->device}
        .setFlags(layoutFlags)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT)
        .initialize(this->layoutFramebuffer);

    uint64_t framebufferSize  = this->layoutFramebuffer.getSize();
    uint32_t framebufferAlign = this->layoutFramebuffer.getAlignment();

    for (unsigned i = 0; i < MAX_FRAMEBUFFERS; i++)
    {
        // Allocate a framebuffer
        this->framebuffers.memory[i] = this->pool.images->allocate(framebufferSize, framebufferAlign);
        this->framebuffers.images[i].initialize(this->layoutFramebuffer,
                                                this->framebuffers.memory[i].getMemBlock(),
                                                this->framebuffers.memory[i].getOffset());

        // Fill in the array for use later by the swapchain creation code
        this->framebufferArray[i] = &this->framebuffers.images[i];
    }

    // Create the swapchain using the framebuffers
    this->swapchain = dk::SwapchainMaker{this->device,
                                         nwindowGetDefault(),
                                         this->framebufferArray}.create();

    this->transformUniformBuffer = this->pool.data->allocate(sizeof(this->transformState), DK_UNIFORM_BUF_ALIGNMENT);
}

// Ensure we have begun our frame
void deko3d::EnsureInFrame()
{
    if (!this->framebuffers.inFrame)
    {
        this->cmdRing.begin(this->cmdBuf);
        this->framebuffers.inFrame = true;
    }
}

/*
** Acquire a framebuffer
** from the swapchain
** (and wait for it to be available)
*/
void deko3d::EnsureHasSlot()
{
    if (this->framebuffers.slot < 0)
        this->framebuffers.slot = this->queue.acquireImage(this->swapchain);
}

/*
** Describe our Vertex struct
** { in, out, structPos, attributeSize, attributeType }
** {  x,   y,         l,             s,             t }
*/
namespace
{
    // Describe the buffer state for the Vertex struct
    constexpr std::array VertexBufferState =
    {
        DkVtxBufferState{ sizeof(vertex::Vertex), 0 },
    };

    constexpr std::array Primitives =
    {
        DkVtxAttribState{ 0, 0, offsetof(vertex::Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
        DkVtxAttribState{ 0, 0, offsetof(vertex::Vertex, color),    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 },
    };
}

/*
** First thing that happens to start the frame
** Clear the screen to a specified color
*/
void deko3d::ClearColor(const Colorf & color)
{
    this->EnsureInFrame();

    this->firstVertex = 0;

    this->cmdBuf.clearColor(0, DkColorMask_RGBA, color.r, color.g, color.b, color.a);

    std::pair<void *, DkGpuAddr> data = this->vtxRing.begin();

    this->vertexData = (vertex::Vertex *)data.first;

    this->cmdBuf.bindRasterizerState(this->state.rasterizer);
    this->cmdBuf.bindColorState(this->state.color);
    this->cmdBuf.bindColorWriteState(this->state.colorWrite);
    this->cmdBuf.bindBlendStates(0, this->state.blendState);

    // Bind the current slice's GPU address to the buffer
    this->cmdBuf.bindVtxBuffer(this->vtxRing.getCurSlice(), data.second, this->vtxRing.getSize());

    this->cmdBuf.bindVtxAttribState(Primitives);
    this->cmdBuf.bindVtxBufferState(VertexBufferState);
}

void deko3d::SetBlendColor(const Colorf & color)
{
    this->cmdBuf.setBlendConst(color.r, color.g, color.b, color.a);
}

/*
** ClearStencil happens *afer* ClearColor
** So this shouldn't have any issues
** Only useful if we have a Stencil buffer
*/
void deko3d::ClearStencil(int stencil)
{
    // this->cmdBuf.clearDepthStencil(false, 1.0f, DkStencilOp_Replace, stencil);
}

/*
** ClearDepth happens *afer* ClearColor -> ClearStencil
** So this shouldn't have any issues
** Only useful if we have a Depth buffer
*/
void deko3d::ClearDepth(double depth)
{
    // this->cmdBuf.clearDepthStencil(true, depth, DkStencilOp_Replace, 1);
}

dk::UniqueDevice & deko3d::GetDevice()
{
    return this->device;
}

std::optional<CMemPool> & deko3d::GetCode()
{
    return this->pool.code;
}


/*
** Binds a Framebuffer we have allocated
** It ensures that there's a "slot" from @EnsureHasSlot
** This is used to access the current Framebuffer image
** TODO: Add depth/stencil images
*/
void deko3d::BindFramebuffer()
{
    // Generate a command list that binds it
    this->EnsureInFrame();

    this->EnsureHasSlot();

    dk::ImageView colorTarget {
        this->framebuffers.images[this->framebuffers.slot]
    };

    this->cmdBuf.bindRenderTargets(&colorTarget);
}

/*
** Presents the current Framebuffer to the screen
** It will call @BindFramebuffer first and submit
** all our commands from the buffer to the queue
*/
void deko3d::Present()
{
    this->cmdBuf.pushConstants(this->transformUniformBuffer.getGpuAddr(),
                               this->transformUniformBuffer.getSize(), 0, sizeof(transformState),
                               &this->transformState);

    // Now that we are done rendering, present it to the screen
    if (this->framebuffers.inFrame)
    {
        // Run the main rendering command list
        this->vtxRing.end();
        this->queue.submitCommands(this->cmdRing.end(this->cmdBuf));

        this->queue.presentImage(this->swapchain, this->framebuffers.slot);

        this->framebuffers.inFrame = false;
    }

    this->framebuffers.slot = -1;
}

/* Various Rendering Operations */

bool deko3d::RenderPolygon(bool fill, const vertex::Vertex * points,
                           size_t count, bool skipLastFilledVertex)
{
    if (count > (this->vtxRing.getSize() - this->firstVertex) || points == nullptr)
        return false;

    if (!fill)
        this->RenderPolyline(points, count);
    else
    {
        int vertexCount = (int)count - (skipLastFilledVertex ? 1 : 0);

        // int drawReq = vertex::GetIndexCount(DkPrimitive_TriangleFan, vertexCount);

        // Copy the vertex info
        memcpy(this->vertexData + this->firstVertex, points, count * sizeof(*points));

        // Draw with Triangles
        this->cmdBuf.draw(DkPrimitive_TriangleFan, vertexCount, 1, this->firstVertex, 0);

        // Offset the first vertex data
        this->firstVertex += vertexCount;
    }

    return true;
}

bool deko3d::RenderPoints(const vertex::Vertex * points, size_t count)
{
    memcpy(this->vertexData + this->firstVertex, points, count * sizeof(*points));

    this->cmdBuf.draw(DkPrimitive_Points, count, 1, this->firstVertex, 0);

    this->firstVertex += count;
}

bool deko3d::RenderPolyline(const vertex::Vertex * points, size_t count)
{
    // int drawReq = vertex::GetIndexCount(DkPrimitive_TriangleFan, vertexCount);

    // Copy the vertex info
    memcpy(this->vertexData + this->firstVertex, points, count * sizeof(*points));

    // Draw with Triangles
    this->cmdBuf.draw(DkPrimitive_TriangleStrip, count, 1, this->firstVertex, 0);

    // Offset the first vertex data
    this->firstVertex += count;

    return true;
}

void deko3d::SetPointSize(float size)
{
    this->cmdBuf.setPointSize(size);
    this->state.pointSize = size;
}

float deko3d::GetPointSize()
{
    return this->state.pointSize;
}

void deko3d::SetColorMask(bool r, bool g, bool b, bool a)
{
    uint32_t masks = 0;

    if (r)
        masks |= DkColorMask_R;

    if (g)
        masks |= DkColorMask_G;

    if (b)
        masks |= DkColorMask_B;

    if (a)
        masks |= DkColorMask_A;

    this->state.colorWrite.setMask(0, masks);
}

void deko3d::SetBlendMode(DkBlendOp func, DkBlendFactor srcColor, DkBlendFactor srcAlpha,
                          DkBlendFactor dstColor, DkBlendFactor dstAlpha)
{
    this->state.blendState.setColorBlendOp(func);

    // Blend factors
    this->state.blendState.setSrcColorBlendFactor(srcColor);
    this->state.blendState.setSrcAlphaBlendFactor(dstAlpha);

    this->state.blendState.setDstColorBlendFactor(dstColor);
    this->state.blendState.setDstAlphaBlendFactor(dstAlpha);
}

void deko3d::SetFrontFaceWinding(DkFrontFace face)
{
    this->state.rasterizer.setFrontFace(face);
}

void deko3d::SetCullMode(DkFace face)
{
    this->state.rasterizer.setCullMode(face);
}

/* Encapsulation and Abstraction - fincs */

/*
** Equivalent to LÖVE's OpenGL::UseProgram function
** We need to make sure that we're *in* a frame before attaching the Shader
** LÖVE attaches a default Shader on Graphics creation, which is not in a frame
** although OpenGL *hides* this from the user, so we have to deal with it in deko3d
*/
void deko3d::UseProgram(const std::pair<CShader, CShader> & program)
{
    this->EnsureInFrame();

    this->cmdBuf.bindShaders(DkStageFlag_GraphicsMask, {program.first, program.second});
    this->cmdBuf.bindUniformBuffer(DkStage_Vertex, 0, this->transformUniformBuffer.getGpuAddr(), this->transformUniformBuffer.getSize());
}

void deko3d::SetDepthWrites(bool enable)
{
    this->state.rasterizer.setDepthClampEnable(enable);
}

// Set the global filter mode for textures
void deko3d::SetFilter(const love::Texture::Filter & filter)
{
    DkFilter min = (filter.min == love::Texture::FILTER_NEAREST) ? DkFilter_Nearest : DkFilter_Linear;
    DkFilter mag = (filter.min == love::Texture::FILTER_NEAREST) ? DkFilter_Nearest : DkFilter_Linear;

    if (filter.mipmap != love::Texture::FILTER_NONE)
    {} // Deal with MipMap

    this->filter.sampler.setFilter(min, mag);
    this->filter.descriptor.initialize(this->filter.sampler);
}

/*
** Set the Scissor region to clip
** Anything drawn outside of this will not be rendered
*/
void deko3d::SetScissor(const love::Rect & scissor, bool canvasActive)
{
    this->EnsureInFrame();

    this->scissor = scissor;
    this->cmdBuf.setScissors(0, { {(uint32_t)scissor.x, (uint32_t)scissor.y,
                                   (uint32_t)scissor.w, (uint32_t)scissor.h} });
}

/*
** Set the viewing screen space for rendering
** This sets up the actual bounds we can see
*/
void deko3d::SetViewport(const love::Rect & view)
{
    this->EnsureInFrame();

    this->viewport = view;
    this->cmdBuf.setViewports(0, { {(float)view.x, (float)view.y,
                                    (float)view.w, (float)view.h,
                                    -10.0f, 10.0f} });
}

Rect deko3d::GetViewport()
{
    return this->viewport;
}

// Singleton
deko3d dk3d;