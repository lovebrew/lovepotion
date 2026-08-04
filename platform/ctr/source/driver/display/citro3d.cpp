#include "common/config.hpp"
#include "common/screen.hpp"

#include "driver/display/citro3d.hpp"
#include "modules/graphics/Shader.hpp"
#include "modules/graphics/vertex.hpp"
#include <3ds/gpu/enums.h>

namespace love
{
    citro3d::citro3d() : context {}
    {
        this->targets.reserve(3);
    }

    void citro3d::init()
    {
        if (this->initialized)
            return;

        gfxInitDefault();

        if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
            throw love::Exception("Failed to initialize citro3d.");

        C3D_AttrInfo* attributes = C3D_GetAttrInfo();
        AttrInfo_Init(attributes);

        AttrInfo_AddLoader(attributes, 0, GPU_FLOAT, 2); //< position
        AttrInfo_AddLoader(attributes, 1, GPU_FLOAT, 2); //< texcoord
        AttrInfo_AddLoader(attributes, 2, GPU_FLOAT, 4); //< color

        this->set3DMode(true);
        this->initialized = true;
    }

    void citro3d::setTexEnvMode(TextureBase* texture, bool isFont)
    {
        TexEnvMode mode = (texture == nullptr) ? TEXENV_MODE_PRIMITIVE : TEXENV_MODE_TEXTURE;
        if (texture && isFont)
            mode = TEXENV_MODE_FONT;

        if (mode == this->context.texEnvMode)
            return;

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

        this->context.texEnvMode = mode;
    }

    void citro3d::close()
    {
        if (!this->initialized)
            return;

        this->destroyFramebuffers();
        C3D_Fini();
        gfxExit();
        this->initialized = false;
    }

    void citro3d::createFramebuffers()
    {
        const auto info              = getScreenInfo();
        const size_t numFramebuffers = info.size();

        this->targets.resize(numFramebuffers);

        try
        {
            for (size_t index = 0; index < numFramebuffers; ++index)
                this->targets[index].create(info[index]);
        }
        catch (...)
        {
            this->destroyFramebuffers();
            throw;
        }
    }

    void citro3d::destroyFramebuffers()
    {
        love::currentScreen = DEFAULT_SCREEN;

        if (this->inFrame)
        {
            C3D_FrameEnd(0);
            this->inFrame = false;
        }

        if (this->isDefaultFramebufferActive())
            this->context.boundFramebuffer = nullptr;

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

    void citro3d::clearColor(const Color& color)
    {
        if (!this->inFrame || !this->context.boundFramebuffer)
            return;

        C3D_RenderTargetClear(this->getFramebuffer(), C3D_CLEAR_COLOR, color.abgr(), 0);
    }

    void citro3d::clear(double depth, int stencil)
    {
        LOVE_UNUSED(stencil);
        if (!this->inFrame || !this->context.boundFramebuffer)
            return;

        const auto clear = uint32_t(std::clamp(depth, 0.0, 1.0) * LOVE_UINT32_MAX);
        C3D_RenderTargetClear(this->getFramebuffer(), C3D_CLEAR_DEPTH, 0, clear);
    }

    void citro3d::setStencilState(const StencilState& state)
    {
        const bool enabled = state.action != STENCIL_KEEP || state.compare != COMPARE_ALWAYS;

        GPU_STENCILOP stencilAction = GPU_STENCIL_KEEP;
        getConstant(state.action, stencilAction);

        GPU_TESTFUNC testFunction = GPU_ALWAYS;
        getConstant(state.compare, testFunction);

        C3D_StencilTest(enabled, testFunction, state.value, state.readMask, state.writeMask);
        C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, stencilAction);
    }

    C3D_RenderTarget* citro3d::getFramebuffer()
    {
        return this->context.boundFramebuffer;
    }

    void citro3d::bindFramebuffer(C3D_RenderTarget* framebuffer)
    {
        if (!framebuffer)
            return;

        if (!this->inFrame)
            this->ensureInFrame();

        bool bindingModified = false;

        if (this->context.boundFramebuffer != framebuffer)
        {
            bindingModified                = true;
            this->context.boundFramebuffer = framebuffer;
        }

        if (bindingModified)
            C3D_FrameDrawOn(framebuffer);
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

        for (size_t index = this->deferred.size(); index > 0; index--)
        {
            this->deferred[index - 1]();
            this->deferred.erase(this->deferred.begin() + index - 1);
        }
    }

    void citro3d::setViewport(const Rect& v)
    {
        this->context.viewport = v;
        C3D_SetViewport((uint32_t)v.x, (uint32_t)v.y, (uint32_t)v.w, (uint32_t)v.h);
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

    void citro3d::setSamplerState(C3D_Tex* texture, SamplerState state)
    {
        auto magFilter = (state.minFilter == SamplerState::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;
        auto minFilter = (state.magFilter == SamplerState::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;

        C3D_TexSetFilter(texture, magFilter, minFilter);

        GPU_TEXTURE_WRAP_PARAM wrapS;
        citro3d::getConstant(state.wrapU, wrapS);

        GPU_TEXTURE_WRAP_PARAM wrapT;
        citro3d::getConstant(state.wrapV, wrapT);

        C3D_TexSetWrap(texture, wrapS, wrapT);

        float maxBias = 0xFFF;

        if (maxBias > 0.01f)
            maxBias = 0.01f;

        state.lodBias = std::clamp(state.lodBias, -maxBias, maxBias);
        C3D_TexSetLodBias(texture, state.lodBias);
    }

    void citro3d::prepareDraw(GraphicsBase* graphics)
    {
        // clang-format off
        if (Shader::current != nullptr)
        {
            Rect viewport = this->context.viewport;
            ((Shader*)Shader::current)->updateBuiltinUniforms(graphics, viewport.w, viewport.h);
        }
        // clang-format on
    }

    void citro3d::bindTextureToUnit(TextureType target, C3D_Tex* texture, int unit)
    {
        C3D_TexBind(0, texture);
    }

    void citro3d::bindTextureToUnit(TextureBase* texture, int unit)
    {
        if (!texture)
            return;

        auto textureType = texture->getTextureType();
        auto* handle     = (C3D_Tex*)texture->getHandle();

        this->bindTextureToUnit(textureType, handle, unit);
    }

    void citro3d::setVertexAttributes(const VertexAttributes& attributes, const BufferBindings& buffers)
    {
        uint32_t i       = 0;
        uint32_t allBits = attributes.enableBits | 3;

        C3D_AttrInfo info {};
        AttrInfo_Init(&info);

        while (allBits)
        {
            uint32_t bit = 1u << i;
            if (attributes.enableBits & bit)
            {
                const auto& attribute = attributes.attributes[i];
                const auto& format    = love::getDataFormatInfo(attribute.getFormat());

                GPU_FORMATS attributeFormat;
                citro3d::getConstant(format.baseType, attributeFormat);

                AttrInfo_AddLoader(&info, i, attributeFormat, format.components);
            }
            i++;
            allBits >>= 1u;
        }

        C3D_SetAttrInfo(&info);
        C3D_SetBufInfo((C3D_BufInfo*)buffers.info[0].buffer->getHandle());
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
