#include "common/runtime.h"
#include "objects/font/font.h"

using namespace love;

bool Font::GetConstant(const char * in, AlignMode & out)
{
    return alignModes.Find(in, out);
}

bool Font::GetConstant(AlignMode in, const char *& out)
{
    return alignModes.Find(in, out);
}

std::vector<std::string> Font::GetConstants(AlignMode)
{
    return alignModes.GetNames();
}

bool Font::GetConstant(const char * in, Font::SystemFontType & out)
{
    return sharedFonts.Find(in, out);
}

bool Font::GetConstant(Font::SystemFontType in, const char *& out)
{
    return sharedFonts.Find(in, out);
}

std::vector<std::string> Font::GetConstants(Font::SystemFontType)
{
    return sharedFonts.GetNames();
}

StringMap<Font::AlignMode, Font::ALIGN_MAX_ENUM>::Entry Font::alignModeEntries[] =
{
    { "left",    ALIGN_LEFT    },
    { "right",   ALIGN_RIGHT   },
    { "center",  ALIGN_CENTER  },
    { "justify", ALIGN_JUSTIFY }
};

StringMap<Font::AlignMode, Font::ALIGN_MAX_ENUM> Font::alignModes(Font::alignModeEntries, sizeof(Font::alignModeEntries));
