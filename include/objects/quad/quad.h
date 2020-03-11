#pragma once

#include "common/mmath.h"

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


        private:
            Viewport viewport;

            double sw;
            double sh;

            double scaledWidth;
            double scaledHeight;
    };
}
