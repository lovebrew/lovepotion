cmake_minimum_required(VERSION 3.13)

# Declare project containing a single eponymous executable
project(lovepotion LANGUAGES C CXX ASM)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Generate symbol list and map information (useful for debugging)
dkp_target_generate_symbol_list(${PROJECT_NAME})

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE COMMIT_HASH
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

include(FetchContent)

set(LOVE_VERSION "11.4.0")
set(APP_VERSION  "3.0.0")

set(APP_TITLE  "LÖVE Potion")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(APP_TITLE "LÖVE Potion (${COMMIT_HASH})")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __DEBUG__=1)
endif()

set(APP_AUTHOR "LÖVEBrew Team")

file(COPY libraries/luasocket DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# arm-none-eabi-cmake
if(NINTENDO_3DS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        __CONSOLE__="3DS" __OS__="Horizon"
    )

    set(APP_ICON platform/ctr/icon.png)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(APP_ICON platform/ctr/icon-dev.png)
    endif()

    # Generate a SMDH file for the executable
    ctr_generate_smdh(${PROJECT_NAME}.smdh
        NAME        "${APP_TITLE}"

        # Other options available:
        DESCRIPTION "LÖVE for 3DS • ${APP_VERSION}"
        AUTHOR      "${APP_AUTHOR}"
        ICON        ${APP_ICON}
    )

    add_subdirectory(platform/ctr)

    # Specify that the executable is a 3DSX file
    ctr_create_3dsx(${PROJECT_NAME}
        SMDH  ${PROJECT_NAME}.smdh
    )

    set(APP_LIBS citro2d citro3d)

    add_custom_target(dist COMMAND
        ${CMAKE_COMMAND} -E tar "cfv" "${PROJECT_NAME}-3ds-${COMMIT_HASH}.zip" --format=zip
            "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.3dsx"
            "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.elf"
    )

    add_custom_target(test
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.3dsx"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test"
        COMMAND ${CMAKE_COMMAND} -E tar "cfv" "game.love" --format=zip .
        COMMAND ${CMAKE_COMMAND} -E cat "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.3dsx" "game.love" > "${CMAKE_CURRENT_BINARY_DIR}/lovepotion_test.3dsx"
    )

    execute_process(COMMAND patch -d ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket -N -i ${PROJECT_SOURCE_DIR}/platform/ctr/libraries/luasocket.patch)
endif()

# aarch64-none-elf-cmake
if(NINTENDO_SWITCH)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        __CONSOLE__="Switch" __OS__="Horizon"
    )

    nx_generate_nacp(${PROJECT_NAME}.nacp
        NAME    "${APP_TITLE}"
        AUTHOR  ${APP_AUTHOR}
        VERSION ${APP_VERSION}
    )

    add_subdirectory(platform/hac)

    set(APP_ICON platform/hac/icon.jpg)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(APP_ICON platform/hac/icon-dev.jpg)
    endif()

    nx_create_nro(${PROJECT_NAME}
        ICON  ${APP_ICON}
        NACP  ${PROJECT_NAME}.nacp
        ROMFS ${PROJECT_NAME}_hac_romfs
    )

    set(APP_LIBS deko3d freetype)

    add_custom_target(dist COMMAND
        ${CMAKE_COMMAND} -E tar "cfv" "${PROJECT_NAME}-switch-${COMMIT_HASH}.zip" --format=zip
            "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.nro"
            "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.elf"
    )

    add_custom_target(test
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.nro"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test"
        COMMAND ${CMAKE_COMMAND} -E tar "cfv" "game.love" --format=zip .
        COMMAND ${CMAKE_COMMAND} -E cat "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.nro" "game.love" > "${CMAKE_CURRENT_BINARY_DIR}/lovepotion_test.nro"
    )

    execute_process(COMMAND patch -d ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket -N -i ${PROJECT_SOURCE_DIR}/platform/hac/libraries/luasocket.patch)
endif()

# powerpc-eabi-cmake
if (NINTENDO_WIIU)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        __CONSOLE__="Wii U" __OS__="Cafe"
    )

    set(APP_ICON platform/cafe/icon.png)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(APP_ICON platform/cafe/icon-dev.png)
    endif()

    add_subdirectory(platform/cafe)

    wut_create_rpx(${PROJECT_NAME})

    wut_create_wuhb(${PROJECT_NAME}
        NAME       "${APP_TITLE}"

        # Bundle a content folder (optional)
        CONTENT    ${PROJECT_NAME}_cafe_content

        # Other options available:
        SHORTNAME  "${APP_TITLE}"
        AUTHOR     ${APP_AUTHOR}
        ICON       ${APP_ICON}
        #TVSPLASH   myTvSplash.png
        #DRCSPLASH  myDrcSplash.png
    )

    add_custom_target(dist COMMAND
        ${CMAKE_COMMAND} -E tar "cfv" "${PROJECT_NAME}-wiiu-${COMMIT_HASH}.zip" --format=zip
            "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.rpx"
            "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.wuhb"
            "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.elf"
    )

    add_custom_target(test
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.wuhb"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test"
        COMMAND ${CMAKE_COMMAND} -E tar "cfv" "game.love" --format=zip .
        COMMAND ${CMAKE_COMMAND} -E cat "${CMAKE_CURRENT_BINARY_DIR}/lovepotion.wuhb" "game.love" > "${CMAKE_CURRENT_BINARY_DIR}/lovepotion_test.wuhb"
    )

    execute_process(COMMAND patch -d ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket -N -i ${PROJECT_SOURCE_DIR}/platform/cafe/libraries/luasocket.patch)

    find_package(SDL2 REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${DEVKITPRO}/portlibs/wiiu/include)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_Mixer mpg123 SDL2)
endif()

# Options for code generation
target_compile_options(${PROJECT_NAME} PRIVATE
    # Common C/C++ options
    -Wall -Wno-psabi

    # C++ specific options
    $<$<COMPILE_LANGUAGE:CXX>:-fexceptions -fno-rtti>
)

# external libraries #

# ddsparse
add_library(ddsparse
    libraries/ddsparse/ddsinfo.h
    libraries/ddsparse/ddsparse.cpp
    libraries/ddsparse/ddsparse.h
)

# lua51
add_library(lua51
    libraries/lua/bit.c
    libraries/lua/lapi.c
    libraries/lua/lapi.h
    libraries/lua/lauxlib.c
    libraries/lua/lauxlib.h
    libraries/lua/lbaselib.c
    libraries/lua/lcode.c
    libraries/lua/lcode.h
    libraries/lua/ldblib.c
    libraries/lua/ldebug.c
    libraries/lua/ldebug.h
    libraries/lua/ldo.c
    libraries/lua/ldo.h
    libraries/lua/ldump.c
    libraries/lua/lfunc.c
    libraries/lua/lfunc.h
    libraries/lua/lgc.c
    libraries/lua/lgc.h
    libraries/lua/linit.c
    libraries/lua/liolib.c
    libraries/lua/llex.c
    libraries/lua/llex.h
    libraries/lua/llimits.h
    libraries/lua/lmathlib.c
    libraries/lua/lmem.c
    libraries/lua/lmem.h
    libraries/lua/loadlib.c
    libraries/lua/lobject.c
    libraries/lua/lobject.h
    libraries/lua/lopcodes.c
    libraries/lua/lopcodes.h
    libraries/lua/loslib.c
    libraries/lua/lparser.c
    libraries/lua/lparser.h
    libraries/lua/lstate.c
    libraries/lua/lstate.h
    libraries/lua/lstring.c
    libraries/lua/lstring.h
    libraries/lua/lstrlib.c
    libraries/lua/ltable.c
    libraries/lua/ltable.h
    libraries/lua/ltablib.c
    libraries/lua/ltm.c
    libraries/lua/ltm.h
    libraries/lua/lua.h
    libraries/lua/luaconf.h
    libraries/lua/lualib.h
    libraries/lua/lundump.c
    libraries/lua/lundump.h
    libraries/lua/lvm.c
    libraries/lua/lvm.h
    libraries/lua/lzio.c
    libraries/lua/lzio.h
    libraries/lua/print.c
)

# lua53-compat
add_library(lua53
    libraries/lua53/l53strlib.c
    libraries/lua53/l53strlib.h
    libraries/lua53/lprefix.h
    libraries/lua53/lutf8lib.c
    libraries/lua53/lutf8lib.h
)
target_include_directories(lua53 PRIVATE
    libraries/lua
)

add_library(luahttps
    libraries/luahttps/common/Connection.h
    libraries/luahttps/common/ConnectionClient.h
    libraries/luahttps/common/HTTPRequest.cpp
    libraries/luahttps/common/HTTPRequest.h
    libraries/luahttps/common/HTTPSClient.cpp
    libraries/luahttps/common/HTTPSClient.h
    libraries/luahttps/common/HTTPSCommon.cpp
    libraries/luahttps/common/HTTPSCommon.h
    libraries/luahttps/common/PlaintextConnection.cpp
    libraries/luahttps/common/PlaintextConnection.h
    libraries/luahttps/generic/CurlClient.cpp
    libraries/luahttps/generic/CurlClient.h
    libraries/luahttps/https.cpp
)
target_include_directories(luahttps PRIVATE
    libraries/lua
)

# luasocket
add_library(luasocket
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/luasocket.hpp
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/luasocket.cpp
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/auxiliar.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/auxiliar.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/buffer.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/buffer.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/compat.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/compat.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/except.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/except.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/inet.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/inet.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/io.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/io.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/ltn12.lua
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/luasocket.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/luasocket.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/mime.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/mime.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/options.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/options.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/pierror.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/select.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/select.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/smtp.lua
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/socket.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/tcp.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/tcp.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/timeout.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/timeout.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/udp.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/udp.h
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/usocket.c
     ${CMAKE_CURRENT_BINARY_DIR}/luasocket/libluasocket/usocket.h
)
target_include_directories(luasocket PRIVATE
    libraries/lua
    libraries/lua53
)

# noise1234
add_library(noise1234
    libraries/noise1234/noise1234.cpp
    libraries/noise1234/noise1234.h
    libraries/noise1234/simplexnoise1234.cpp
    libraries/noise1234/simplexnoise1234.h
)

# libwuff
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


target_compile_definitions(${PROJECT_NAME} PRIVATE
    __APP_VERSION__=\"${VERSION}\" __LOVE_VERSION__=\"${LOVE_VERSION}\"
)

# find PkgConfig for liblz4, libmpg123
find_package(PkgConfig REQUIRED)

# link lz4
pkg_check_modules(liblz4 REQUIRED IMPORTED_TARGET liblz4)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::liblz4)

# link curl
find_package(CURL REQUIRED)
target_link_libraries(luahttps PRIVATE CURL::libcurl)

# link everything else
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${APP_LIBS} physfs box2d z jpeg png vorbisidec ogg
    modplug lua51 lua53 wuff ddsparse luahttps luasocket noise1234
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    libraries/ddsparse
    libraries/dr
    libraries/lua
    libraries/lua53
    libraries/luahttps
    libraries/luasocket
    libraries/noise1234
    libraries/utf8
    libraries/wuff
)

# Source code files
# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/common/console.cpp
    source/common/data.cpp
    source/common/exception.cpp
    source/common/luax.cpp
    source/common/module.cpp
    source/common/object.cpp
    source/common/pixelformat.cpp
    source/common/type.cpp
    source/common/variant.cpp
    source/common/vector.cpp
    source/main.cpp
    source/modules/audio/audio.cpp
    source/modules/audio/wrap_audio.cpp
    source/modules/data/data.cpp
    source/modules/data/wrap_data.cpp
    source/modules/event/event.cpp
    source/modules/event/wrap_event.cpp
    source/modules/filesystem/filesystem.cpp
    source/modules/filesystem/physfs/filesystem.cpp
    source/modules/filesystem/wrap_filesystem.cpp
    source/modules/font/wrap_fontmodule.cpp
    source/modules/graphics/wrap_graphics.cpp
    source/modules/image/imagemodule.cpp
    source/modules/image/wrap_imagemodule.cpp
    source/modules/joystick/wrap_joystickmodule.cpp
    source/modules/keyboard/wrap_keyboard.cpp
    source/modules/love/love.cpp
    source/modules/sound/sound.cpp
    source/modules/sound/wrap_sound.cpp
    source/modules/system/wrap_system.cpp
    source/modules/thread/threadmodule.cpp
    source/modules/thread/wrap_threadmodule.cpp
    source/modules/timer/wrap_timer.cpp
    source/modules/touch/touch.cpp
    source/modules/touch/wrap_touch.cpp
    source/modules/window/wrap_window.cpp
    source/objects/channel/channel.cpp
    source/objects/channel/wrap_channel.cpp
    source/objects/data/bytedata/bytedata.cpp
    source/objects/data/bytedata/wrap_bytedata.cpp
    source/objects/data/compresseddata/compresseddata.cpp
    source/objects/data/compresseddata/wrap_compresseddata.cpp
    source/objects/data/dataview/dataview.cpp
    source/objects/data/dataview/wrap_dataview.cpp
    source/objects/data/filedata/filedata.cpp
    source/objects/data/filedata/wrap_filedata.cpp
    source/objects/data/sounddata/sounddata.cpp
    source/objects/data/sounddata/wrap_sounddata.cpp
    source/objects/data/wrap_data.cpp
    source/objects/decoder/wrap_decoder.cpp
    source/objects/file/file.cpp
    source/objects/file/physfs/file.cpp
    source/objects/file/wrap_file.cpp
    source/objects/font/wrap_font.cpp
    source/objects/glyphdata/glyphdata.cpp
    source/objects/glyphdata/wrap_glyphdata.cpp
    source/objects/imagedata/wrap_imagedata.cpp
    source/objects/joystick/wrap_joystick.cpp
    source/objects/rasterizer/wrap_rasterizer.cpp
    source/objects/source/wrap_source.cpp
    source/objects/thread/luathread.cpp
    source/objects/thread/wrap_luathread.cpp
    source/utilities/base64.cpp
    source/utilities/bytes.cpp
    source/utilities/compressor/compressor.cpp
    source/utilities/compressor/types/lz4compressor.cpp
    source/utilities/compressor/types/zlibcompressor.cpp
    source/utilities/decoder/decoder.cpp
    source/utilities/decoder/types/flacdecoder.cpp
    source/utilities/decoder/types/modplugdecoder.cpp
    source/utilities/decoder/types/mp3decoder.cpp
    source/utilities/decoder/types/vorbisdecoder.cpp
    source/utilities/decoder/types/wavedecoder.cpp
    source/utilities/driver/renderer/renderstate.cpp
    source/utilities/driver/renderer/samplerstate.cpp
    source/utilities/formathandler/formathandler.cpp
    source/utilities/formathandler/types/jpghandler.cpp
    source/utilities/formathandler/types/pnghandler.cpp
    source/utilities/guid.cpp
    source/utilities/hashfunction/hashfunction.cpp
    source/utilities/hashfunction/types/md5.cpp
    source/utilities/hashfunction/types/sha1.cpp
    source/utilities/hashfunction/types/sha256.cpp
    source/utilities/hashfunction/types/sha512.cpp
    source/utilities/log/logfile.cpp
    source/utilities/pool/poolthread.cpp
    source/utilities/pool/sources.cpp
    source/utilities/pool/vibrations.cpp
    source/utilities/result.cpp
    source/utilities/stream/stream.cpp
    source/utilities/stream/types/datastream.cpp
    source/utilities/threads/threadable.cpp
)
