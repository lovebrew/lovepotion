#pragma once

#include "objects/quad/quad.h"
#include "common/stringmap.h"
#include "common/exception.h"
#include "common/strongref.h"
#include "common/mmath.h"

#include "objects/drawable/drawable.h"

#if defined (_3DS)
    #define LOVE_SetFilter(texture, mag, min) \
        C3D_TexSetFilter((texture.tex), (GPU_TEXTURE_FILTER_PARAM)(mag), (GPU_TEXTURE_FILTER_PARAM)(min))
    #define LOVE_SetWrapMode(texture, s, t) \
        C3D_TexSetWrap((texture.tex), (GPU_TEXTURE_WRAP_PARAM)(s), (GPU_TEXTURE_WRAP_PARAM)(t))
#elif defined (__SWITCH__)
    #define LOVE_SetFilter(texture, mag, min)
    #define LOVE_SetWrapMode(texture, s, t)
#endif

namespace love
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

            Texture(TextureType texType);
            virtual ~Texture();

            TextureType GetTextureType() const;

            int GetWidth() const;
            int GetHeight() const;

            virtual void SetFilter(const Filter & f);
            virtual const Filter & GetFilter() const;

            virtual bool SetWrap(const Wrap & w);
            virtual const Wrap & GetWrap() const;

            void Draw(Graphics * gfx, const Matrix4 & localTransform) override;
            virtual void Draw(Graphics * gfx, Quad * quad, const Matrix4 & localTransform);

            Quad * GetQuad() const;

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
            TextureHandle texture;

            int width;
            int height;

            Filter filter;
            Wrap wrap;

            StrongReference<Quad> quad;

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
