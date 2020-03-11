/*
** display.h
** Handles internal display information/rendering
*/

#pragma once

namespace Display
{
    /* Variables */

    inline bool open = false;
    inline std::vector<Renderer *> targets = { nullptr };
    inline Frame * window = nullptr;

    inline Color m_blendColor = { 1, 1, 1, 1 };

    /* Functions */

    void Initialize();

    void Clear(Color * color);
    int Present(lua_State * L);

    inline void SetBlendColor(Color * color) {
        m_blendColor = {color->r, color->g, color->b, color->a};
    }

    inline Color GetBlendColor() {
        return m_blendColor;
    }

    int SetScreen(lua_State * L);

    std::vector<std::pair<int, int>> & GetWindowSizes();
    unsigned int GetDisplayCount();

    Renderer * GetRenderer();

    bool IsOpen();

    void Exit();
};
