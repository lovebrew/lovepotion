#include "modules/graphics/Video.hpp"
#include "modules/graphics/Graphics.tcc"

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

        stream->fillBackBuffer();

        for (int i = 0; i < 4; i++)
            this->vertices[i].color = Color(1, 1, 1, 1);

        vertices[0].x = 0.0f;
        vertices[0].y = 0.0f;
        vertices[1].x = 0.0f;
        vertices[1].y = (float)height;
        vertices[2].x = (float)width;
        vertices[2].y = 0.0f;
        vertices[3].x = (float)width;
        vertices[3].y = (float)height;

        vertices[0].s = 0.0f;
        vertices[0].t = 0.0f;
        vertices[1].s = 0.0f;
        vertices[1].t = 1.0f;
        vertices[2].s = 1.0f;
        vertices[2].t = 0.0f;
        vertices[3].s = 1.0f;
        vertices[3].t = 1.0f;

        auto frame     = (const VideoStream::Frame*)stream->getFrontBuffer();
        int widths[3]  = { frame->yw, frame->cw, frame->cw };
        int heights[3] = { frame->yh, frame->ch, frame->ch };

        const unsigned char* data[3] = { frame->yplane, frame->cbplane, frame->crplane };

        TextureBase::Settings settings {};

        for (int i = 0; i < 3; i++)
        {
            settings.width  = Console::is(Console::CTR) ? this->getWidth() : widths[i];
            settings.height = Console::is(Console::CTR) ? this->getHeight() : heights[i];
            settings.format = Console::is(Console::CTR) ? PIXELFORMAT_RGBA8_UNORM : PIXELFORMAT_R8_UNORM;

            TextureBase* texture = graphics->newTexture(settings, nullptr);
            texture->setSamplerState(this->samplerState);

            if constexpr (!Console::is(Console::CTR))
            {
                size_t bpp  = getPixelFormatBlockSize(PIXELFORMAT_R8_UNORM);
                size_t size = bpp * widths[i] * heights[i];

                Rect rect = { 0, 0, widths[i], heights[i] };
                texture->replacePixels(data[i], size, 0, 0, rect, false);
            }
            else
            {
                // TODO: just replace the handle?
            }

            this->textures[i].set(texture, Acquire::NO_RETAIN);
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

    void Video::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        this->update();

        auto shader = ShaderBase::current;
        if (!Console::is(Console::CTR) && ShaderBase::isDefaultActive())
            shader = ShaderBase::standardShaders[ShaderBase::STANDARD_VIDEO];

        // TODO: set video textures in shader?

        const Matrix4& tm = graphics->getTransform();
        bool is2D         = tm.isAffine2DTransform();

        Matrix4 translated(tm, matrix);

        BatchedDrawCommand command {};
        command.format      = CommonFormat::XYf_STf_RGBAf;
        command.indexMode   = TRIANGLEINDEX_QUADS;
        command.vertexCount = 4;
        command.shaderType  = ShaderBase::getVideoShader();

        auto data    = graphics->requestBatchedDraw(command);
        auto* stream = (XYf_STf_RGBAf*)data.stream;

        if (is2D)
            translated.transformXY((Vector2*)stream, vertices, 4);
        else
            translated.transformXY0((Vector3*)stream, vertices, 4);

        for (int i = 0; i < 4; i++)
        {
            stream[i].s     = vertices[i].s;
            stream[i].t     = vertices[i].t;
            stream[i].color = vertices[i].color;
        }

        graphics->flushBatchedDraws();
    }

    void Video::update()
    {
        bool buffersChanged = stream->swapBuffers();
        stream->fillBackBuffer();

        if (buffersChanged)
        {
            auto frame = (const VideoStream::Frame*)stream->getFrontBuffer();

            if constexpr (!Console::is(Console::CTR))
            {
                int widths[3]  = { frame->yw, frame->cw, frame->cw };
                int heights[3] = { frame->yh, frame->ch, frame->ch };

                const unsigned char* data[3] = { frame->yplane, frame->cbplane, frame->crplane };

                for (int i = 0; i < 3; i++)
                {
                    size_t bpp  = getPixelFormatBlockSize(PIXELFORMAT_R8_UNORM);
                    size_t size = bpp * widths[i] * heights[i];

                    Rect rect = { 0, 0, widths[i], heights[i] };
                    textures[i]->replacePixels(data[i], size, 0, 0, rect, false);
                }
            }
            else
            {
                // TODO: just replace the handle?
            }
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
