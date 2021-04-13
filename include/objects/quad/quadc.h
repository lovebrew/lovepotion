#pragma once

#include "common/lmath.h"
#include "common/vector.h"

#include "objects/object.h"

namespace love::common
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

        Quad(double sw, double sh);

        virtual ~Quad();

        /* LOVE FUNCTIONS */

        double GetTextureWidth() const;

        double GetTextureHeight() const;

        void SetViewport(const Viewport& viewport);

        const Viewport& GetViewport() const;

        /* END LOVE FUNCTIONS */

        void RefreshViewport(const Viewport& viewport, double sw, double sh);

        virtual void Refresh(const Viewport& viewport, double sw, double sh) = 0;

      protected:
        Viewport viewport;

        double sw;
        double sh;
    };
} // namespace love::common
