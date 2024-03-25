cmake_minimum_required(VERSION 3.13)
project(lovepotion LANGUAGES C CXX)

add_executable(${PROJECT_NAME})
dkp_target_generate_symbol_list(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE COMMIT_HASH
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

set(LOVE_VERSION "12.0")
set(APP_VERSION  "3.1.0")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    __APP_VERSION__=\"${APP_VERSION}\" __LOVE_VERSION__=\"${LOVE_VERSION}\"
)

set(APP_TITLE  "LÖVE Potion")
set(APP_AUTHOR "LÖVEBrew Team")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(APP_TITLE "LÖVE Potion (${COMMIT_HASH})")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __DEBUG__=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE __DEBUG__=0)
endif()

set(COMPILE_OPTIMIZATION -O2)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(COMPILE_OPTIMIZATION -Og)
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "COMPILE_OPTIMIZATION: ${COMPILE_OPTIMIZATION}")

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wno-psabi -Wextra ${COMPILE_OPTIMIZATION}
    $<$<COMPILE_LANGUAGE:CXX>:-fexceptions -fno-rtti>
)


if(NINTENDO_3DS)
    add_subdirectory(platform/ctr)

    target_compile_definitions(${PROJECT_NAME} PRIVATE
        __CONSOLE__="3DS" __OS__="Horizon"
    )

    set(APP_ICON platform/ctr/icon.png)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(APP_ICON platform/ctr/icon-dev.png)
    endif()

    ctr_generate_smdh(${PROJECT_NAME}.smdh
        NAME        "${APP_TITLE}"
        DESCRIPTION "LÖVE for 3DS • ${APP_VERSION}"
        AUTHOR      "${APP_AUTHOR}"
        ICON        ${APP_ICON}
    )

    ctr_create_3dsx(${PROJECT_NAME}
        SMDH  ${PROJECT_NAME}.smdh
        ROMFS ${PROJECT_NAME}_ctr_romfs
    )

    set(APP_LIBS citro3d)
    set(OUTPUT_FILENAME "${PROJECT_NAME}.3dsx")
endif()

if (NINTENDO_SWITCH)
    add_subdirectory(platform/hac)

    target_compile_definitions(${PROJECT_NAME} PRIVATE
        __CONSOLE__="Switch" __OS__="Horizon"
    )

    set(OUTPUT_FILENAME "${PROJECT_NAME}.nro")
endif()

if (NINTENDO_WII_U)
    add_subdirectory(platform/cafe)

    target_compile_definitions(${PROJECT_NAME} PRIVATE
        __CONSOLE__="Wii U" __OS__="Cafe" __LOVE_BIG_ENDIAN__
    )

    set(OUTPUT_FILENAME "${PROJECT_NAME}.wuhb")
endif()

add_custom_target(test
    DEPENDS           "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILENAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests"
    COMMAND           ${CMAKE_COMMAND} -E tar -cvf "game.zip" --format=zip .
    COMMAND           ${CMAKE_COMMAND} -E cat "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILENAME}" "game.zip" > "${CMAKE_CURRENT_SOURCE_DIR}/${OUTPUT_FILENAME}"
    COMMAND           ${CMAKE_COMMAND} -E rm "game.zip"
)

# find PkgConfig for liblz4, libmpg123
find_package(PkgConfig REQUIRED)

pkg_check_modules(liblz4 REQUIRED IMPORTED_TARGET liblz4)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::liblz4)

# lua5.1
pkg_check_modules(lua51 REQUIRED IMPORTED_TARGET lua51)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::lua51)

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
target_link_libraries(${PROJECT_NAME} PRIVATE love_physfs)

# link everything else
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${APP_LIBS} z luabit lua53
)

include_directories(
    include
    libraries/lua53
    libraries/physfs
)

# find source -type f -name \*.cpp | clip
target_sources(${PROJECT_NAME} PRIVATE
source/common/b64.cpp
source/common/Data.cpp
source/common/luax.cpp
source/common/Message.cpp
source/common/Module.cpp
source/common/Object.cpp
source/common/Stream.cpp
source/common/types.cpp
source/common/Variant.cpp
source/main.cpp
source/modules/data/ByteData.cpp
source/modules/data/CompressedData.cpp
source/modules/data/DataModule.cpp
source/modules/data/DataView.cpp
source/modules/data/misc/Compressor.cpp
source/modules/data/misc/HashFunction.cpp
source/modules/data/wrap_ByteData.cpp
source/modules/data/wrap_CompressedData.cpp
source/modules/data/wrap_Data.cpp
source/modules/data/wrap_DataModule.cpp
source/modules/data/wrap_DataView.cpp
source/modules/event/Event.cpp
source/modules/event/wrap_Event.cpp
source/modules/filesystem/FileData.cpp
source/modules/filesystem/physfs/File.cpp
source/modules/filesystem/physfs/Filesystem.cpp
source/modules/filesystem/wrap_File.cpp
source/modules/filesystem/wrap_FileData.cpp
source/modules/filesystem/wrap_Filesystem.cpp
source/modules/love/love.cpp
source/modules/timer/wrap_Timer.cpp
)
