cmake_minimum_required(VERSION 3.13)

project(lovepotion
    LANGUAGES C CXX
    VERSION 3.1.0
    DESCRIPTION "LÖVE For Nintendo Homebrew"
)

add_executable(${PROJECT_NAME})
dkp_target_generate_symbol_list(${PROJECT_NAME})

include(FetchContent)
find_package(Git QUIET REQUIRED)
find_package(Patch REQUIRED)
find_package(PkgConfig REQUIRED)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wno-psabi -Wextra
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Release>:-O2>
    $<$<COMPILE_LANGUAGE:CXX>:-fexceptions -fno-rtti>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    __APP_VERSION__="${PROJECT_VERSION}" __LOVE_VERSION__="12.0"
    $<$<CONFIG:Debug>:__DEBUG__=1>
)

set(APP_TITLE "LÖVE Potion")
string(APPEND ${APP_TITLE} $<$<CONFIG:Debug>:" (${COMMIT_HASH})">)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE COMMIT_HASH
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LUASOCKET_COPY_DIR ${CMAKE_CURRENT_BINARY_DIR})
file(COPY libraries/luasocket DESTINATION ${LUASOCKET_COPY_DIR})

if(NINTENDO_3DS)
    add_subdirectory(platform/ctr)

    target_sources(${PROJECT_NAME} PRIVATE
        source/modules/image/magpie/T3XHandler.cpp
    )
endif()

if (NINTENDO_SWITCH OR NINTENDO_WIIU)
    if (NINTENDO_SWITCH)
        add_subdirectory(platform/hac)
    else()
        add_subdirectory(platform/cafe)
    endif()

    target_sources(${PROJECT_NAME} PRIVATE
        source/modules/image/magpie/ASTCHandler.cpp
        source/modules/image/magpie/ddsHandler.cpp
        source/modules/image/magpie/JPGHandler.cpp
        source/modules/image/magpie/KTXHandler.cpp
        source/modules/image/magpie/PKMHandler.cpp
        source/modules/image/magpie/PNGHandler.cpp
        source/modules/font/freetype/Font.cpp
        source/modules/font/freetype/TrueTypeRasterizer.cpp
        source/modules/graphics/freetype/Font.cpp
    )

    add_library(ddsparse
        libraries/ddsparse/ddsparse.cpp
        libraries/ddsparse/ddsparse.h
        libraries/ddsparse/ddsinfo.h
    )
endif()

pkg_check_modules(liblz4 REQUIRED IMPORTED_TARGET liblz4)
pkg_check_modules(lua51 REQUIRED IMPORTED_TARGET lua51)

# lua51
add_library(luabit
    libraries/luabit/bit.c
)
target_link_libraries(luabit PRIVATE PkgConfig::lua51)

# lua53-compat
add_library(lua53
    libraries/lua53/l53strlib.c
    libraries/lua53/l53strlib.h
    libraries/lua53/lprefix.h
    libraries/lua53/lutf8lib.c
    libraries/lua53/lutf8lib.h
)
target_link_libraries(lua53 PRIVATE PkgConfig::lua51)

add_library(luasocket
     ${LUASOCKET_COPY_DIR}/luasocket/luasocket.hpp
     ${LUASOCKET_COPY_DIR}/luasocket/luasocket.cpp
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/auxiliar.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/auxiliar.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/buffer.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/buffer.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/compat.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/compat.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/except.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/except.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/inet.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/inet.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/io.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/io.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/ltn12.lua
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/luasocket.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/luasocket.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/mime.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/mime.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/options.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/options.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/pierror.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/select.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/select.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/smtp.lua
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/socket.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/tcp.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/tcp.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/timeout.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/timeout.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/udp.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/udp.h
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/usocket.c
     ${LUASOCKET_COPY_DIR}/luasocket/libluasocket/usocket.h
)
target_link_libraries(luasocket PRIVATE PkgConfig::lua51)
target_include_directories(luasocket PRIVATE
    libraries/lua53
)

FetchContent_Declare(lua-https
    GIT_REPOSITORY https://github.com/love2d/lua-https
    GIT_TAG        main
)

FetchContent_GetProperties(lua-https)
if(NOT lua-https_POPULATED)
    FetchContent_MakeAvailable(lua-https)
endif()
set(USE_CURL_BACKEND ON)

pkg_check_modules(libcurl REQUIRED IMPORTED_TARGET libcurl)

add_library(love_physfs
    libraries/physfs/physfs.c
    libraries/physfs/physfs.h
    libraries/physfs/physfs_archiver_7z.c
    libraries/physfs/physfs_archiver_csm.c
    libraries/physfs/physfs_archiver_dir.c
    libraries/physfs/physfs_archiver_grp.c
    libraries/physfs/physfs_archiver_hog.c
    libraries/physfs/physfs_archiver_iso9660.c
    libraries/physfs/physfs_archiver_mvl.c
    libraries/physfs/physfs_archiver_qpak.c
    libraries/physfs/physfs_archiver_slb.c
    libraries/physfs/physfs_archiver_unpacked.c
    libraries/physfs/physfs_archiver_vdf.c
    libraries/physfs/physfs_archiver_wad.c
    libraries/physfs/physfs_archiver_zip.c
    libraries/physfs/physfs_byteorder.c
    libraries/physfs/physfs_casefolding.h
    libraries/physfs/physfs_internal.h
    libraries/physfs/physfs_lzmasdk.h
    libraries/physfs/physfs_miniz.h
    libraries/physfs/physfs_platforms.h
    libraries/physfs/physfs_platform_android.c
    libraries/physfs/physfs_platform_apple.m
    libraries/physfs/physfs_platform_cafe.c
    libraries/physfs/physfs_platform_ctr.c
    libraries/physfs/physfs_platform_hac.c
    libraries/physfs/physfs_platform_haiku.cpp
    libraries/physfs/physfs_platform_ogc.c
    libraries/physfs/physfs_platform_os2.c
    libraries/physfs/physfs_platform_posix.c
    libraries/physfs/physfs_platform_qnx.c
    libraries/physfs/physfs_platform_unix.c
    libraries/physfs/physfs_platform_windows.c
    libraries/physfs/physfs_platform_winrt.cpp
    libraries/physfs/physfs_unicode.c
)

add_library(wuff
    libraries/wuff/wuff.c
    libraries/wuff/wuff.h
    libraries/wuff/wuff_config.h
    libraries/wuff/wuff_convert.c
    libraries/wuff/wuff_convert.h
    libraries/wuff/wuff_internal.c
    libraries/wuff/wuff_internal.h
    libraries/wuff/wuff_memory.c
)

add_library(noise1234
    libraries/noise1234/noise1234.cpp
    libraries/noise1234/noise1234.h
    libraries/noise1234/simplexnoise1234.cpp
    libraries/noise1234/simplexnoise1234.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${APP_LIBS} z luabit lua53 ogg modplug https PkgConfig::libcurl luasocket
    love_physfs wuff noise1234 PkgConfig::lua51 PkgConfig::liblz4
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    platform
    libraries/dr
    libraries/lua53
    libraries/noise1234
    libraries/luasocket
    libraries/physfs
    libraries/wuff
    libraries/utf8
    libraries/ddsparse
)

# find source -type f -name \*.cpp | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/main.cpp
    source/common/b64.cpp
    source/common/Data.cpp
    source/common/debug.cpp
    source/common/float.cpp
    source/common/luax.cpp
    source/common/Matrix.cpp
    source/common/Message.cpp
    source/common/Module.cpp
    source/common/Object.cpp
    source/common/pixelformat.cpp
    source/common/Reference.cpp
    source/common/screen.cpp
    source/common/Stream.cpp
    source/common/types.cpp
    source/common/Variant.cpp
    source/utility/guid.cpp
    source/modules/audio/Audio.cpp
    source/modules/audio/Pool.cpp
    source/modules/audio/Source.cpp
    source/modules/audio/wrap_Audio.cpp
    source/modules/audio/wrap_Source.cpp
    source/modules/data/ByteData.cpp
    source/modules/data/CompressedData.cpp
    source/modules/data/DataModule.cpp
    source/modules/data/DataStream.cpp
    source/modules/data/DataView.cpp
    source/modules/data/wrap_ByteData.cpp
    source/modules/data/wrap_CompressedData.cpp
    source/modules/data/wrap_Data.cpp
    source/modules/data/wrap_DataModule.cpp
    source/modules/data/wrap_DataView.cpp
    source/modules/event/Event.cpp
    source/modules/event/wrap_Event.cpp
    source/modules/filesystem/FileData.cpp
    source/modules/filesystem/File.cpp
    source/modules/filesystem/Filesystem.cpp
    source/modules/filesystem/NativeFile.cpp
    source/modules/filesystem/physfs/PhysfsIO.cpp
    source/modules/filesystem/wrap_File.cpp
    source/modules/filesystem/wrap_FileData.cpp
    source/modules/filesystem/wrap_Filesystem.cpp
    source/modules/filesystem/wrap_NativeFile.cpp
    source/modules/font/Font.cpp
    source/modules/font/GlyphData.cpp
    source/modules/font/Rasterizer.cpp
    source/modules/font/TextShaper.cpp
    source/modules/font/GenericShaper.cpp
    source/modules/font/wrap_Font.cpp
    source/modules/font/wrap_Rasterizer.cpp
    source/modules/font/wrap_GlyphData.cpp
    source/modules/graphics/ShaderStage.cpp
    source/modules/graphics/Graphics.cpp
    source/modules/graphics/FontBase.cpp
    source/modules/graphics/Polyline.cpp
    source/modules/graphics/TextBatch.cpp
    source/modules/graphics/renderstate.cpp
    source/modules/graphics/samplerstate.cpp
    source/modules/graphics/Shader.cpp
    source/modules/graphics/SpriteBatch.cpp
    source/modules/graphics/wrap_SpriteBatch.cpp
    source/modules/graphics/Quad.cpp
    source/modules/graphics/Volatile.cpp
    source/modules/graphics/vertex.cpp
    source/modules/graphics/wrap_Graphics.cpp
    source/modules/graphics/wrap_Texture.cpp
    source/modules/graphics/wrap_TextBatch.cpp
    source/modules/graphics/wrap_Font.cpp
    source/modules/graphics/wrap_Quad.cpp
    source/modules/graphics/Texture.cpp
    source/modules/image/CompressedImageData.cpp
    source/modules/image/CompressedSlice.cpp
    source/modules/image/FormatHandler.cpp
    source/modules/image/Image.cpp
    source/modules/image/ImageData.cpp
    source/modules/image/ImageDataBase.cpp
    source/modules/image/wrap_CompressedImageData.cpp
    source/modules/image/wrap_Image.cpp
    source/modules/image/wrap_ImageData.cpp
    source/modules/joystick/JoystickModule.cpp
    source/modules/joystick/wrap_Joystick.cpp
    source/modules/joystick/wrap_JoystickModule.cpp
    source/modules/keyboard/wrap_Keyboard.cpp
    source/modules/love/love.cpp
    source/modules/math/BezierCurve.cpp
    source/modules/math/MathModule.cpp
    source/modules/math/RandomGenerator.cpp
    source/modules/math/Transform.cpp
    source/modules/math/wrap_BezierCurve.cpp
    source/modules/math/wrap_MathModule.cpp
    source/modules/math/wrap_RandomGenerator.cpp
    source/modules/math/wrap_Transform.cpp
    source/modules/sensor/Sensor.cpp
    source/modules/sensor/wrap_Sensor.cpp
    source/modules/sound/Decoder.cpp
    source/modules/sound/Sound.cpp
    source/modules/sound/SoundData.cpp
    source/modules/sound/wrap_Decoder.cpp
    source/modules/sound/wrap_Sound.cpp
    source/modules/sound/wrap_SoundData.cpp
    source/modules/system/wrap_System.cpp
    source/modules/thread/Channel.cpp
    source/modules/thread/LuaThread.cpp
    source/modules/thread/Thread.cpp
    source/modules/thread/Threadable.cpp
    source/modules/thread/ThreadModule.cpp
    source/modules/thread/wrap_Channel.cpp
    source/modules/thread/wrap_LuaThread.cpp
    source/modules/thread/wrap_Thread.cpp
    source/modules/timer/wrap_Timer.cpp
    source/modules/touch/Touch.cpp
    source/modules/touch/wrap_Touch.cpp
    source/modules/window/wrap_Window.cpp
    source/modules/window/Window.cpp
    source/modules/data/misc/Compressor.cpp
    source/modules/data/misc/HashFunction.cpp
    source/modules/filesystem/physfs/File.cpp
    source/modules/filesystem/physfs/Filesystem.cpp
    source/modules/sound/lullaby/FLACDecoder.cpp
    source/modules/sound/lullaby/ModPlugDecoder.cpp
    source/modules/sound/lullaby/MP3Decoder.cpp
    source/modules/sound/lullaby/VorbisDecoder.cpp
    source/modules/sound/lullaby/WaveDecoder.cpp
)
