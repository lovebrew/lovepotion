#include <modules/fontmodule_ext.hpp>

using namespace love;

FontModule<Console::HAC>::FontModule()
{
    try
    {
        this->defaultFont = std::make_shared<SystemFont>();
    }
    catch (std::bad_alloc& e)
    {
        throw love::Exception("Failed to create default system font!");
    }

    if (auto error = FT_Init_FreeType(&this->library); error != FT_Err_Ok)
        throw love::Exception("Failed to initialize FreeType: (%s)", FT_Error_String(error));
}

FontModule<Console::HAC>::~FontModule()
{
    FT_Done_FreeType(this->library);
}
