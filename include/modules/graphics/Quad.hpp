#pragma once

#include "common/Object.hpp"
#include "common/Vector.hpp"
#include "common/math.hpp"

namespace love
{
    class Quad : public Object
    {
      public:
        static Type type;

        struct Viewport
        {
            double x, y;
            double w, h;
        };

        Quad(const Viewport& viewport, double sourceWidth, double sourceHeight);

        virtual ~Quad();

        void refresh(const Viewport& viewport, double sourceWidth, double sourceHeight);

        void setViewport(const Viewport& viewport)
        {
            this->refresh(viewport, this->sourceWidth, this->sourceHeight);
        }

        void setTextureCoordinate(int index, const Vector2& coordinate)
        {
            this->textureCoordinates[index] = coordinate;
        }

        Viewport getViewport() const
        {
            return this->viewport;
        }

        void setLayer(int layer)
        {
            this->layer = layer;
        }

        int getLayer() const
        {
            return this->layer;
        }

        double getTextureWidth() const
        {
            return this->sourceWidth;
        }

        double getTextureHeight() const
        {
            return this->sourceHeight;
        }

        const Vector2* getVertexPositions() const
        {
            return this->vertexPositions;
        }

        const Vector2* getTextureCoordinates() const
        {
            return this->textureCoordinates;
        }

      private:
        Vector2 vertexPositions[4];
        Vector2 textureCoordinates[4];

        Viewport viewport;
        double sourceWidth;
        double sourceHeight;

        int layer;
    };
} // namespace love
