#pragma once

#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#endif

#include <vector>

typedef uint8_t RenderScreen;

namespace love::common
{
    class Screen
    {
      protected:
        template<typename T>
        static bool FindSetCast(const auto& BiMap, const char* in, RenderScreen& value)
        {
            T temp       = static_cast<T>(0);
            bool success = BiMap.Find(in, temp);

            value = static_cast<RenderScreen>(temp);
            return success;
        }

        template<typename T>
        static bool ReverseFindSetCast(const auto& BiMap, RenderScreen& in, const char*& value)
        {
            T temp       = static_cast<T>(0);
            bool success = BiMap.ReverseFind(temp, value);

            return success;
        }

        void SetActiveScreen(RenderScreen screen)
        {
            this->current = screen;
        }

        virtual int GetWidth(RenderScreen screen = 0) = 0;

        virtual int GetHeight() = 0;

      protected:
        RenderScreen current;
    };
} // namespace love::common
