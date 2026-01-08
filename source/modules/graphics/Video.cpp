#include "common/Console.hpp"

#include "modules/graphics/DrawCommand.hpp"
#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Video.hpp"

#include "common/debug.hpp"

namespace love
{
    Type Video::type("Video", &Drawable::type);

    Video::Video(GraphicsBase* graphics, VideoStream* stream, float dpiScale) :
        stream(stream),
        width(stream->getWidth() / dpiScale),
        height(stream->getHeight() / dpiScale),
        samplerState()
    {
        const auto& defaultSampler       = graphics->getDefaultSamplerState();
        this->samplerState.minFilter     = defaultSampler.minFilter;
        this->samplerState.magFilter     = defaultSampler.magFilter;
        this->samplerState.wrapU         = defaultSampler.wrapU;
        this->samplerState.wrapV         = defaultSampler.wrapV;
        this->samplerState.maxAnisotropy = defaultSampler.maxAnisotropy;

        this->stream->fillBackBuffer();

        for (int index = 0; index < this->vertices.size(); index++)
            this->vertices[index].color = Color(1.0f, 1.0f, 1.0f, 1.0f);

        // Vertices are ordered for use with triangle strips:
        // 0---2
        // | / |
        // 1---3

        const auto pWidth  = Console::is(Console::CTR) ? NextPo2(width) : width;
        const auto pHeight = Console::is(Console::CTR) ? NextPo2(height) : height;

        this->vertices[0].x = 0.0f;
        this->vertices[0].y = 0.0f;
        this->vertices[1].x = 0.0f;
        this->vertices[1].y = (float)pHeight;
        this->vertices[2].x = (float)pWidth;
        this->vertices[2].y = 0.0f;
        this->vertices[3].x = (float)pWidth;
        this->vertices[3].y = (float)pHeight;

        this->vertices[0].s = 0.0f;
        this->vertices[0].t = 0.0f;
        this->vertices[1].s = 0.0f;
        this->vertices[1].t = 1.0f;
        this->vertices[2].s = 1.0f;
        this->vertices[2].t = 0.0f;
        this->vertices[3].s = 1.0f;
        this->vertices[3].t = 1.0f;

#if defined(__3DS__)
        for (int index = 0; index < this->vertices.size(); index++)
            this->vertices[index].t = 1.0f - this->vertices[index].t;
#endif

        auto* frame                      = (const VideoStream::Frame*)stream->getFrontBuffer();
        const std::array<int, 3> widths  = { frame->yw, frame->cw, frame->cw };
        const std::array<int, 3> heights = { frame->yh, frame->ch, frame->ch };

#if !defined(__3DS__)
        const std::array<uint8_t*, 3> data = { frame->yPlane, frame->cbPlane, frame->crPlane };
#else
        const std::array<uint8_t*, 1> data = { frame->data };
#endif

        TextureBase::Settings settings {};
        const auto format = Console::is(Console::CTR) ? PIXELFORMAT_RGBA8_UNORM : PIXELFORMAT_R8_UNORM;

        for (int index = 0; index < data.size(); index++)
        {
            settings.width  = widths[index];
            settings.height = heights[index];
            settings.format = format;

            auto* texture = graphics->newTexture(settings, nullptr);
            texture->setSamplerState(this->samplerState);

            size_t size = getPixelFormatSliceSize(format, widths[index], heights[index]);

            Rect rect = { 0, 0, widths[index], heights[index] };
            texture->replacePixels(data[index], size, 0, 0, rect, false);
            this->textures[index].set(texture, Acquire::NO_RETAIN);
        }
    }

    Video::~Video()
    {
        if (this->source)
            this->source->stop();
    }

    VideoStream* Video::getStream()
    {
        return this->stream;
    }

    static constexpr auto SHADER_TYPE =
        Console::is(Console::CTR) ? ShaderBase::STANDARD_DEFAULT : ShaderBase::STANDARD_VIDEO;

    void Video::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        this->update();

        auto shader = ShaderBase::current;
        if (ShaderBase::isDefaultActive() && !Console::is(Console::CTR))
            shader = ShaderBase::standardShaders[ShaderBase::STANDARD_VIDEO];

        const Matrix4& tm = graphics->getTransform();
        bool is2D         = tm.isAffine2DTransform();

        Matrix4 translated(tm, matrix);

        BatchedDrawCommand command {};
        command.format      = CommonFormat::XYf_STf_RGBAf;
        command.indexMode   = TRIANGLEINDEX_QUADS;
        command.vertexCount = 4;
        command.shaderType  = SHADER_TYPE;
        command.texture     = this->textures[0];

        BatchedVertexData data = graphics->requestBatchedDraw(command);
        XYf_STf_RGBAf* stream  = (XYf_STf_RGBAf*)data.stream;

        if (is2D)
            translated.transformXY(stream, this->vertices.data(), 4);

        for (int index = 0; index < 4; index++)
        {
            stream[index].s     = this->vertices[index].s;
            stream[index].t     = this->vertices[index].t;
            stream[index].color = graphics->getColor();
        }

        graphics->flushBatchedDraws();
    }

    void Video::update()
    {
        bool changed = this->stream->swapBuffers();
        this->stream->fillBackBuffer();

        const auto format = Console::is(Console::CTR) ? PIXELFORMAT_RGBA8_UNORM : PIXELFORMAT_R8_UNORM;

        if (changed)
        {
            auto* frame = (const VideoStream::Frame*)this->stream->getFrontBuffer();

            std::array<int, 3> widths  = { frame->yw, frame->cw, frame->cw };
            std::array<int, 3> heights = { frame->yh, frame->ch, frame->ch };

#if !defined(__3DS__)
            const std::array<uint8_t*, 3> data = { frame->yPlane, frame->cbPlane, frame->crPlane };
#else
            size_t size = getPixelFormatSliceSize(format, widths[0], heights[0]);
            Rect rect   = { 0, 0, widths[0], heights[0] };
            this->textures[0]->replacePixels(frame->data, size, 0, 0, rect, false);
#endif
        }
    }

    SourceBase* Video::getSource()
    {
        return this->source;
    }

    void Video::setSource(SourceBase* source)
    {
        this->source = source;
    }

    int Video::getWidth() const
    {
        return this->width;
    }

    int Video::getHeight() const
    {
        return this->height;
    }

    int Video::getPixelWidth() const
    {
        return this->stream->getWidth();
    }

    int Video::getPixelHeight() const
    {
        return this->stream->getHeight();
    }

    void Video::setSamplerState(const SamplerState& state)
    {
        this->samplerState.minFilter     = state.minFilter;
        this->samplerState.magFilter     = state.magFilter;
        this->samplerState.wrapU         = state.wrapU;
        this->samplerState.wrapV         = state.wrapV;
        this->samplerState.maxAnisotropy = state.maxAnisotropy;

        for (const auto& texture : this->textures)
            texture->setSamplerState(this->samplerState);
    }

    const SamplerState& Video::getSamplerState() const
    {
        return this->samplerState;
    }
} // namespace love
