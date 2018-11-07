#pragma once

class Canvas : public Drawable
{
    public:
        Canvas(int width, int height);
        Canvas() {};

        void SetAsTarget();
};