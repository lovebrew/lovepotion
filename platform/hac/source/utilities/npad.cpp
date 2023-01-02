#include <utilities/npad.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

using namespace love;

HidNpadStyleTag love::npad::GetStyleTag(PadState* state)
{
    uint32_t styleSet   = padGetStyleSet(state);
    uint32_t attributes = padGetAttributes(state);

    if (styleSet & HidNpadStyleTag_NpadFullKey)
        return HidNpadStyleTag_NpadFullKey;
    else if (styleSet & HidNpadStyleTag_NpadHandheld)
        return HidNpadStyleTag_NpadHandheld;

    /* this won't work with PadState split and join at the moment */
    if (styleSet & HidNpadStyleTag_NpadJoyLeft)
        return HidNpadStyleTag_NpadJoyLeft;

    if (styleSet & HidNpadStyleTag_NpadJoyDual)
    {
        if (!(attributes & HidNpadAttribute_IsLeftConnected))
            return HidNpadStyleTag_NpadJoyRight;
        else
            return HidNpadStyleTag_NpadJoyDual;
    }

    return INVALID_STYLE_TAG;
}
