#pragma once

#include "common/math.hpp"

#include "modules/graphics/Drawable.hpp"
#include "modules/graphics/Texture.tcc"
#include "modules/graphics/vertex.hpp"

#include "modules/audio/Source.hpp"
#include "modules/video/VideoStream.hpp"

#include <array>

namespace love
{
    class GraphicsBase;

    class Video : public Drawable
    {
      public:
        static Type type;

        Video(GraphicsBase* graphics, VideoStream* stream, float dpiScale = 1.0f);

        virtual ~Video();

        void draw(GraphicsBase* graphics, const Matrix4& matrix) override;

        VideoStream* getStream();

        SourceBase* getSource();

        void setSource(SourceBase* source);

        int getWidth() const;

        int getHeight() const;

        int getPixelWidth() const;

        int getPixelHeight() const;

        void setSamplerState(const SamplerState& state);

        const SamplerState& getSamplerState() const;

      private:
        void update();

        StrongRef<VideoStream> stream;

        int width;
        int height;

        SamplerState samplerState;
        std::array<Vertex, 4> vertices;

        std::array<StrongRef<TextureBase>, 3> textures;
        StrongRef<SourceBase> source;
    };
} // namespace love
