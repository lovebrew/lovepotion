#include "modules/filesystem/wrap_NativeFile.hpp"
#include "modules/filesystem/wrap_File.hpp"

namespace love
{
    int open_nativefile(lua_State* L)
    {
        return luax_register_type(L, &NativeFile::type, Wrap_File::w_File_functions);
    }
} // namespace love
