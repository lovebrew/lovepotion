/*
** modules/graphics.h
** @brief : Used for rendering graphics (primitives, images, text)
*/

#pragma once

class Graphics
{
    public:
        Graphics() = delete;

        static void Initialize();

        static gfxScreen_t GetScreen();

        static Color GetBackgroundColor();

        static u32 ConvertColor(Color & color);

        static std::array<GPU_TEXTURE_FILTER_PARAM, 2> GetFilters();

        static int Register(lua_State * L);

        static void Exit();

    private:
        static inline std::array<GPU_TEXTURE_FILTER_PARAM, 2> filter = { GPU_LINEAR };

        static inline Color blendColor = { 1, 1, 1, 1 };
        static inline Color backgColor = { 0, 0, 0, 1 };

        static inline gfxScreen_t screen = GFX_TOP;

        static inline float depth = 0.0f;

        static inline float alpha = 1.0f;

        static inline bool STACK_PUSHED = false;

        static inline std::vector<StackMatrix> transform = { };

        static inline std::array<const char *, 4> gpuInfo = {};

        static void Transform(float * originalX, float * originalY, float * originalRotation, float * originalScalarX, float * originalScalarY);

        //Löve2D Functions

        static int Clear(lua_State * L);
        static int Present(lua_State * L);

        static int Circle(lua_State * L);
        static int Rectangle(lua_State * L);

        static int Set3D(lua_State * L);
        static int SetDepth(lua_State * L);

        static int Draw(lua_State * L);
        static int Print(lua_State * L);

        static int SetBackgroundColor(lua_State * L);
        static int SetColor(lua_State * L);
        static int SetDefaultFilter(lua_State * L);

        static int GetDimensions(lua_State * L);
        static int GetWidth(lua_State * L);
        static int GetHeight(lua_State * L);

        static int GetRendererInfo(lua_State * L);

        static int SetScreen(lua_State * L);

        static int GetFont(lua_State * L);
        static int SetFont(lua_State * L);
        static int SetNewFont(lua_State * L);

        static int SetScissor(lua_State * L);

        static int Push(lua_State * L);
        static int Pop(lua_State * L);

        static int Translate(lua_State * L);
        static int Scale(lua_State * L);
        static int Rotate(lua_State * L);
        static int Origin(lua_State * L);

        //End Löve2D Functions
};
