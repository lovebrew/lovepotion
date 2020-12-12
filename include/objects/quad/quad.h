#pragma once

#include "common/mmath.h"
#include "common/vector.h"

#include "objects/object.h"
namespace love
{
    class Quad : public Object
    {
        public:
            struct Viewport
            {
                double x, y;
                double w, h;
            };

            static love::Type type;

            Quad(const Viewport & viewport, double sw, double sh);
            ~Quad();

            /* LOVE FUNCTIONS */

            double GetTextureWidth(bool scaled = false) const;

            double GetTextureHeight(bool scaled = false) const;

            Viewport GetViewport() const;

            void SetViewport(const Viewport & viewport);

            /* END LOVE FUNCTIONS */

            void Refresh(const Viewport & viewport, double sw, double sh);

            const Vector2 * GetVertexPositions() const { return vertexPositions; }
            const Vector2 * GetVertexTexCoords() const { return vertexTexCoords; }

        private:
            Viewport viewport;

            Vector2 vertexPositions[4];
            Vector2 vertexTexCoords[4];

            double sw;
            double sh;

            double scaledWidth;
            double scaledHeight;
    };
}
