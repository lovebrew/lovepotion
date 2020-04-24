#pragma once

#if defined(_3DS)
    typedef SwkbdState LOVE_Swkbd;
    #define KEYBOARD_STRING_MAX_LEN 0x100
#elif defined (__SWITCH__)
    typedef SwkbdConfig LOVE_Swkbd;
    #define KEYBOARD_STRING_MAX_LEN 0x1F4
#endif

#define KEYBOARD_STRING_DEFAULT_LEN 0x14

namespace love
{
    class Keyboard : public Module
    {
        public:
            struct SwkbdOpt
            {
                std::string type;
                bool isPassword;
                std::string hint;

                u32 maxLength;
            };

            Keyboard();

            ModuleType GetModuleType() const { return M_KEYBOARD; }

            const char * GetName() const override { return "love.keyboard"; }

            /* LOVE2D Functions */

            std::string SetTextInput(const SwkbdOpt & options);

        private:
            LOVE_Swkbd keyboard;
    };
}
