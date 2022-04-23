#pragma once

#include "objects/drawable/drawable.h"

#include "objects/source/source.h"
#include "objects/videostream/videostream.h"

#include "objects/image/image.h"
#include "objects/texture/texture.h"

namespace love
{
    class Graphics;

    namespace common
    {
        class Video : public Drawable
        {
          public:
            static love::Type type;

            Video(Graphics* graphics, VideoStream* stream, float dpiScale = 1.0f);

            virtual ~Video();

            virtual void Draw(Graphics* graphics, const Matrix4& matrix) = 0;

            VideoStream* GetStream();

            love::Source* GetSource();

            void SetSource(love::Source* source);

            int GetWidth() const;

            int GetHeight() const;

            int GetPixelWidth() const;

            int GetPixelHeight() const;

            void SetFilter(const Texture::Filter& filter);

            const Texture::Filter& GetFilter() const;

          protected:
            virtual void Update() = 0;

            StrongReference<VideoStream> stream;

            int width;
            int height;

            Texture::Filter filter;

            StrongReference<love::Source> source;
            StrongReference<Image> images[3];
        };
    } // namespace common
} // namespace love
