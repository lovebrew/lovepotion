#pragma once

#include <common/console.hpp>
#include <common/math.hpp>
#include <common/object.hpp>
#include <common/vector.hpp>

namespace love
{
    class Quad : public Object
    {
      public:
        static inline Type type = Type("Quad", &Object::type);

        struct Viewport
        {
            double x, y;
            double w, h;
        };

        Quad(const Viewport& viewport, double sourceWidth, double sourceHeight);

        virtual ~Quad();

        void Refresh(const Viewport& viewport, double sourceWidth, double sourceHeight);

        void SetViewport(const Viewport& viewport);

        Viewport GetViewport() const;

        double GetTextureWidth() const;

        double GetTextureHeight() const;

        const Vector2* GetVertexPositions() const;

        const Vector2* GetVertexTextureCoords() const;

        void SetLayer(int layer);

        int GetLayer() const;

      private:
        Vector2 vertexPositions[4];
        Vector2 vertexTextureCoords[4];

        int arrayLayer;
        Viewport viewport;

        double sourceWidth;
        double sourceHeight;
    };
} // namespace love
