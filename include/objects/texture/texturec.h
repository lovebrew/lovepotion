#pragma once

#include "objects/quad/quad.h"
#include "common/stringmap.h"
#include "common/exception.h"
#include "common/strongref.h"
#include "common/mmath.h"

#include "objects/drawable/drawable.h"

#if defined (__SWITCH__)
    #define LOVE_SetFilter(texture, mag, min)
    #define LOVE_SetWrapMode(texture, s, t)

    #include "deko3d/common.h"
#endif

namespace love::common
{
    class Texture : public Drawable
    {
        public:
            enum TextureType
            {
                TEXTURE_2D,
                TEXTURE_MAX_ENUM
            };

            enum WrapMode
            {
                WRAP_CLAMP,
                WRAP_CLAMP_ZERO,
                WRAP_REPEAT,
                WRAP_MIRRORED_REPEAT,
                WRAP_MAX_ENUM
            };

            enum FilterMode
            {
                FILTER_NEAREST,
                FILTER_LINEAR,
                FILTER_NONE,
                FILTER_MAX_ENUM
            };

            struct Filter
            {
                FilterMode min = FILTER_LINEAR;
                FilterMode mag = FILTER_LINEAR;

                FilterMode mipmap = FILTER_NONE;
                float anisotropy  = 1.0f;
            };

            struct Wrap
            {
                WrapMode s = WRAP_CLAMP_ZERO;
                WrapMode t = WRAP_CLAMP_ZERO;
                WrapMode r = WRAP_CLAMP_ZERO;
            };

            static love::Type type;

            static Filter defaultFilter;
            static FilterMode defaultMipmapFilter;
            static float defaultMipmapSharpness;

            Texture(TextureType texType);
            virtual ~Texture();

            TextureType GetTextureType() const;

            int GetWidth(int mip = 0) const;
            int GetHeight(int mip = 0) const;

            virtual void SetFilter(const Filter & f) = 0;
            virtual const Filter & GetFilter() const;

            virtual bool SetWrap(const Wrap & w) = 0;
            virtual const Wrap & GetWrap() const;

            virtual void Draw(Graphics * gfx, const Matrix4 & localTransform) = 0;
            virtual void Draw(Graphics * gfx, love::Quad * quad, const Matrix4 & localTransform) = 0;

            love::Quad * GetQuad() const;

            static bool GetConstant(const char * in, TextureType & out);
            static bool GetConstant(TextureType in, const char *& out);
            static std::vector<std::string> GetConstants(TextureType t);

            static bool GetConstant(const char * in, FilterMode & out);
            static bool GetConstant(FilterMode in, const char *& out);
            static std::vector<std::string> GetConstants(FilterMode f);

            static bool GetConstant(const char * in, WrapMode & out);
            static bool GetConstant(WrapMode in, const char *& out);
            static std::vector<std::string> GetConstants(WrapMode w);

        protected:
            TextureType texType;

            int width;
            int height;

            Filter filter;
            Wrap wrap;

            StrongReference<love::Quad> quad;

            void InitQuad();

        private:
            static StringMap<TextureType, TEXTURE_MAX_ENUM>::Entry texTypeEntries[];
            static StringMap<TextureType, TEXTURE_MAX_ENUM> texTypes;

            static StringMap<FilterMode, FILTER_MAX_ENUM>::Entry filterModeEntries[];
            static StringMap<FilterMode, FILTER_MAX_ENUM> filterModes;

            static StringMap<WrapMode, WRAP_MAX_ENUM>::Entry wrapModeEntries[];
            static StringMap<WrapMode, WRAP_MAX_ENUM> wrapModes;

    };
}
