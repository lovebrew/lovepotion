#include "common/screen.hpp"

#include "driver/display/citro3d.hpp"
#include "modules/graphics/Shader.hpp"

namespace love
{
    citro3d::citro3d() : context {}
    {
        this->targets.reserve(3);
    }

    void citro3d::initialize()
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

        this->initialized = true;
    }

    void citro3d::updateTexEnvMode(TexEnvMode mode)
    {
        if (mode == this->context.texEnvMode || mode == TEXENV_MODE_MAX_ENUM)
            return;

        this->context.texEnvMode = mode;

        C3D_TexEnv* env = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env);

        switch (mode)
        {
            default:
            case TEXENV_MODE_PRIMITIVE:
                C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
                break;
            case TEXENV_MODE_TEXTURE:
                C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
                break;
            case TEXENV_MODE_FONT:
            {
                C3D_TexEnvSrc(env, C3D_RGB, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);

                C3D_TexEnvSrc(env, C3D_Alpha, GPU_PRIMARY_COLOR, GPU_TEXTURE0, GPU_PRIMARY_COLOR);
                C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
                break;
            }
        }
    }

    void citro3d::setupContext()
    {
        for (int i = 0; i < TEXTURE_MAX_ENUM + 1; i++)
        {
            this->context.boundTextures[i].clear();
            this->context.boundTextures[i].resize(3, nullptr);
        }

        this->bindFramebuffer(this->targets[0].get());
    }

    citro3d::~citro3d()
    {
        this->destroyFramebuffers();

        C3D_Fini();
        gfxExit();
    }

    void citro3d::createFramebuffers()
    {
        const auto& info             = getScreenInfo();
        const size_t numFramebuffers = info.size();

        for (size_t index = 0; index < numFramebuffers; ++index)
        {
            Framebuffer target {};
            target.create(info[index]);

            this->targets.push_back(std::move(target));
        }
    }

    void citro3d::destroyFramebuffers()
    {
        for (auto& target : this->targets)
            target.destroy();
    }

    void citro3d::ensureInFrame()
    {
        if (!this->inFrame)
        {
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            this->inFrame = true;
        }
    }

    bool citro3d::isDefaultFramebufferActive() const
    {
        for (size_t index = 0; index < this->targets.size(); index++)
        {
            if (this->targets[index].get() == this->context.boundFramebuffer)
                return true;
        }

        return false;
    }

    C3D_RenderTarget* citro3d::getInternalBackbuffer() const
    {
        return this->targets[love::currentScreen].get();
    }

    void citro3d::clear(const Color& color)
    {
        if (!this->inFrame)
            return;

        C3D_RenderTargetClear(this->getFramebuffer(), C3D_CLEAR_ALL, color.abgr(), 0);
    }

    void citro3d::clearDepthStencil(int depth, uint8_t mask, double stencil)
    {
        /// ???
    }

    C3D_RenderTarget* citro3d::getFramebuffer()
    {
        return this->context.boundFramebuffer;
    }

    void citro3d::bindFramebuffer(C3D_RenderTarget* framebuffer)
    {
        bool bindingModified = false;

        if (this->context.boundFramebuffer != framebuffer)
        {
            bindingModified                = true;
            this->context.boundFramebuffer = framebuffer;
        }

        if (bindingModified)
        {
            C3D_FrameDrawOn(framebuffer);
            this->setViewport(framebuffer->frameBuf.height, framebuffer->frameBuf.width, framebuffer->linked);
        }
    }

    void citro3d::present()
    {
        if (this->inFrame)
        {
            Graphics::flushBatchedDrawsGlobal();

            C3D_FrameEnd(0);

            Graphics::advanceStreamBuffersGlobal();
            this->inFrame = false;
        }
    }

    void citro3d::setViewport(int width, int height, bool tilt)
    {
        this->context.dirtyProjection = true;

        if (height == GSP_SCREEN_WIDTH && tilt)
        {
            if (width == GSP_SCREEN_HEIGHT_TOP || width == GSP_SCREEN_HEIGHT_TOP_2X)
            {
                Mtx_Copy(&this->context.projection, &this->targets[0].getProjection());
                return;
            }
            else if (width == GSP_SCREEN_HEIGHT_BOTTOM)
            {
                const auto index = gfxIs3D() ? 2 : 1;
                Mtx_Copy(&this->context.projection, &this->targets[index].getProjection());
                return;
            }
        }

        // clang-format off
        auto* ortho = tilt ? Mtx_OrthoTilt : Mtx_Ortho;
        ortho(&this->context.projection, 0.0f, width, height, 0.0f, Framebuffer::Z_NEAR, Framebuffer::Z_FAR, true);
        // clang-format on

        C3D_SetViewport(0, 0, (uint32_t)width, (uint32_t)height);
    }

    void citro3d::setScissor(const Rect& scissor)
    {
        const int width  = this->context.boundFramebuffer->frameBuf.height;
        const int height = this->context.boundFramebuffer->frameBuf.width;

        Framebuffer::calculateBounds(scissor, this->context.scissor, width, height);

        // clang-format off
        GPU_SCISSORMODE mode = (scissor != Rect::EMPTY) ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;
        C3D_SetScissor(mode, this->context.scissor.y, this->context.scissor.x, this->context.scissor.h, this->context.scissor.w);
        // clang-format on
    }

    void citro3d::setCullMode(CullMode mode)
    {
        GPU_CULLMODE cullMode;
        if (!citro3d::getConstant(mode, cullMode))
            return;

        if (this->context.cullMode == mode)
            return;

        C3D_CullFace(cullMode);
        this->context.cullMode = mode;
    }

    void citro3d::setVertexWinding(Winding)
    {}

    void citro3d::setColorMask(ColorChannelMask mask)
    {
        uint8_t writeMask = GPU_WRITE_DEPTH;
        writeMask |= mask.get();

        if (this->context.colorMask == mask)
            return;

        this->context.colorMask = mask;
        C3D_DepthTest(true, GPU_GEQUAL, (GPU_WRITEMASK)writeMask);
    }

    void citro3d::setBlendState(const BlendState& state)
    {
        if (this->context.blendState == state)
            return;

        GPU_BLENDEQUATION operationRGB;
        if (!citro3d::getConstant(state.operationRGB, operationRGB))
            return;

        GPU_BLENDEQUATION operationA;
        if (!citro3d::getConstant(state.operationA, operationA))
            return;

        GPU_BLENDFACTOR sourceColor;
        if (!citro3d::getConstant(state.srcFactorRGB, sourceColor))
            return;

        GPU_BLENDFACTOR destColor;
        if (!citro3d::getConstant(state.dstFactorRGB, destColor))
            return;

        GPU_BLENDFACTOR sourceAlpha;
        if (!citro3d::getConstant(state.srcFactorA, sourceAlpha))
            return;

        GPU_BLENDFACTOR destAlpha;
        if (!citro3d::getConstant(state.dstFactorA, destAlpha))
            return;

        this->context.blendState = state;
        C3D_AlphaBlend(operationRGB, operationA, sourceColor, destColor, sourceAlpha, destAlpha);
    }

    void citro3d::setSamplerState(C3D_Tex* texture, SamplerState state)
    {
        auto magFilter = (state.minFilter == SamplerState::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;
        auto minFilter = (state.magFilter == SamplerState::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;

        C3D_TexSetFilter(texture, magFilter, minFilter);

        auto wrapU = citro3d::getWrapMode(state.wrapU);
        auto wrapV = citro3d::getWrapMode(state.wrapV);

        C3D_TexSetWrap(texture, wrapU, wrapV);

        float maxBias = 0xFFF;

        if (maxBias > 0.01f)
            maxBias = 0.01f;

        state.lodBias = std::clamp(state.lodBias, -maxBias, maxBias);
        C3D_TexSetLodBias(texture, state.lodBias);
    }

    void citro3d::prepareDraw()
    {
        // clang-format off
        if (Shader::current != nullptr && this->context.dirtyProjection)
        {
            ((Shader*)Shader::current)->updateBuiltinUniforms(this->context.modelView, this->context.projection);
            this->context.dirtyProjection = false;
        }
        // clang-format on
    }

    void citro3d::bindTextureToUnit(TextureType target, C3D_Tex* texture, int unit, bool restorePrevious,
                                    bool bindForEdit)
    {
        if (texture == nullptr)
            return this->updateTexEnvMode(TEXENV_MODE_PRIMITIVE);

        int oldTextureUnit = this->context.currentTextureUnit;

        int activeUnit = 0;
        if (oldTextureUnit != unit)
            activeUnit = getTextureUnit(GPU_TEXUNIT0) + unit;

        this->updateTexEnvMode(TEXENV_MODE_TEXTURE);
        this->context.boundTextures[target][unit] = texture;

        C3D_TexBind(activeUnit, texture);

        if (restorePrevious && oldTextureUnit != unit)
            activeUnit = getTextureUnit(GPU_TEXUNIT0) + unit;
        else
            this->context.currentTextureUnit = unit;
    }

    void citro3d::bindTextureToUnit(TextureBase* texture, int unit, bool restorePrevious, bool bindForEdit)
    {
        if (texture == nullptr)
            return this->updateTexEnvMode(TEXENV_MODE_PRIMITIVE);

        auto textureType = texture->getTextureType();
        auto* handle     = (C3D_Tex*)texture->getHandle();

        this->bindTextureToUnit(textureType, handle, unit, restorePrevious, bindForEdit);
    }

    void citro3d::setVertexAttributes(const VertexAttributes& attributes, const BufferBindings& buffers)
    {}

    GPU_TEXTURE_WRAP_PARAM citro3d::getWrapMode(SamplerState::WrapMode mode)
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

    GPU_Primitive_t citro3d::getPrimitiveType(PrimitiveType type)
    {
        switch (type)
        {
            case PRIMITIVE_TRIANGLES:
                return GPU_TRIANGLES;
            case PRIMITIVE_TRIANGLE_FAN:
                return GPU_TRIANGLE_FAN;
            case PRIMITIVE_TRIANGLE_STRIP:
                return GPU_TRIANGLE_STRIP;
            default:
                break;
        }

        throw love::Exception("Invalid primitive type: {:d}.", (int)type);
    }

    int citro3d::getTextureUnit(GPU_TEXUNIT unit)
    {
        switch (unit)
        {
            case GPU_TEXUNIT0:
            default:
                return 0;
            case GPU_TEXUNIT1:
                return 1;
            case GPU_TEXUNIT2:
                return 2;
        }

        throw love::Exception("Invalid texture unit: {:d}.", (int)unit);
    }

    citro3d c3d;
} // namespace love
