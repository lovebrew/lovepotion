#include "common/screen.hpp"

#include "driver/display/Renderer.hpp"
#include "modules/graphics/Shader.hpp"

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

        C3D_AttrInfo* attributes = C3D_GetAttrInfo();
        AttrInfo_Init(attributes);

        AttrInfo_AddLoader(attributes, 0, GPU_FLOAT, 2); //< position
        AttrInfo_AddLoader(attributes, 1, GPU_FLOAT, 2); //< texcoord
        AttrInfo_AddLoader(attributes, 2, GPU_FLOAT, 4); //< color

        Mtx_Identity(&this->context.modelView);
        Mtx_Identity(&this->context.projection);

        this->set3DMode(true);

        this->batchedDrawState.vertices     = (Vertex*)linearAlloc(sizeof(Vertex) * LOVE_UINT16_MAX);
        this->batchedDrawState.verticesSize = LOVE_UINT16_MAX;

        if (!this->batchedDrawState.vertices)
            throw love::Exception("Failed to allocate vertex buffer.");

        BufInfo_Init(&this->context.buffer);

        if (BufInfo_Add(&this->context.buffer, this->batchedDrawState.vertices, sizeof(Vertex), 3, 0x210) < 0)
        {
            linearFree(this->batchedDrawState.vertices);
            throw love::Exception("Failed to initialize vertex buffer.");
        }

        C3D_SetBufInfo(&this->context.buffer);

        this->initialized = true;
    }

    Renderer::~Renderer()
    {
        this->destroyFramebuffers();

        linearFree(this->batchedDrawState.vertices);

        C3D_Fini();
        gfxExit();
    }

    void Renderer::createFramebuffers()
    {
        const auto& info          = getScreenInfo();
        const int numFramebuffers = info.size();

        for (size_t index = 0; index < numFramebuffers; ++index)
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
        this->flushBatchedDraws();

        this->context.target = this->targets[currentScreen];
        auto viewport        = this->context.target.getViewport();

        C3D_FrameDrawOn(this->context.target.get());
        this->setViewport(viewport, this->context.target.get()->linked);
    }

    void Renderer::present()
    {
        if (this->inFrame)
        {
            this->flushBatchedDraws();

            C3D_FrameEnd(0);
            this->inFrame = false;
        }
    }

    void Renderer::updateUniforms()
    {
        if (this->context.dirtyProjection)
        {
            if (Shader::current != nullptr)
            {
                auto uniforms = Shader::current->getUniforms();

                C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uniforms.projMtx, &this->context.projection);
                C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uniforms.mdlvMtx, &this->context.modelView);
            }

            this->context.dirtyProjection = false;
        }
    }

    void Renderer::setViewport(const Rect& viewport, bool tilt)
    {
        this->context.viewport        = viewport;
        this->context.dirtyProjection = true;

        if (viewport.h == GSP_SCREEN_WIDTH && tilt)
        {
            if (viewport.w == GSP_SCREEN_HEIGHT_TOP || viewport.w == GSP_SCREEN_HEIGHT_TOP_2X)
            {
                Mtx_Copy(&this->context.projection, &this->targets[0].getProjection());
                return;
            }
            else if (viewport.w == GSP_SCREEN_HEIGHT_BOTTOM)
            {
                const auto index = gfxIs3D() ? 2 : 1;
                Mtx_Copy(&this->context.projection, &this->targets[index].getProjection());
                return;
            }
        }

        // clang-format off
        auto* ortho = tilt ? Mtx_OrthoTilt : Mtx_Ortho;
        ortho(&this->context.projection, 0.0f, viewport.w, viewport.h, 0.0f, Framebuffer::Z_NEAR, Framebuffer::Z_FAR, true);
        // clang-format on

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

    void Renderer::setVertexWinding(Winding)
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

    void Renderer::setSamplerState(C3D_Tex* texture, SamplerState state)
    {
        auto magFilter = (state.minFilter == SamplerState::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;
        auto minFilter = (state.magFilter == SamplerState::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;

        C3D_TexSetFilter(texture, magFilter, minFilter);

        auto wrapU = Renderer::getWrapMode(state.wrapU);
        auto wrapV = Renderer::getWrapMode(state.wrapV);

        C3D_TexSetWrap(texture, wrapU, wrapV);
    }

    void Renderer::draw(const DrawIndexedCommand& command)
    {
        if (command.texture != nullptr)
        {
            this->setAttribute(TEXENV_TEXTURE);
            C3D_TexBind(0, (C3D_Tex*)command.texture->getHandle());
        }
        else
            this->setAttribute(TEXENV_PRIMITIVE);

        GPU_Primitive_t primitiveType;
        if (!Renderer::getConstant(command.primitiveType, primitiveType))
            throw love::Exception("Invalid primitive type: {:d}.", (int)command.primitiveType);

        decltype(C3D_UNSIGNED_BYTE) indexType;
        if (!Renderer::getConstant(command.indexType, indexType))
            throw love::Exception("Invalid index type: {:d}.", (int)command.indexType);

        const void* elements = (const void*)command.indexBufferOffset;
        C3D_DrawElements(primitiveType, command.indexCount, indexType, elements);
    }

    GPU_TEXTURE_WRAP_PARAM Renderer::getWrapMode(SamplerState::WrapMode mode)
    {
        switch (mode)
        {
            case SamplerState::WRAP_CLAMP:
                return GPU_CLAMP_TO_EDGE;
            case SamplerState::WRAP_REPEAT:
                return GPU_REPEAT;
            case SamplerState::WRAP_MIRRORED_REPEAT:
                return GPU_MIRRORED_REPEAT;
            default:
                return GPU_CLAMP_TO_EDGE;
        }
    }
} // namespace love
