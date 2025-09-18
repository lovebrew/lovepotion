dkp_add_asset_target(${PROJECT_NAME}_cafe_content content)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    __CONSOLE__="wiiu" __OS__="cafe" LOVE_BIG_ENDIAN
    __RENDERER_NAME__="GX2" __RENDERER_VERSION__="1.0.0"
    __RENDERER_VENDOR__="AMD" __RENDERER_DEVICE__="GPU7"
)

if (Patch_FOUND)
    message(STATUS "Patching LuaSocket for Wii U")
    execute_process(COMMAND ${Patch_EXECUTABLE} -d ${LUASOCKET_COPY_DIR}/luasocket/libluasocket -p1 -N -i ${CMAKE_CURRENT_SOURCE_DIR}/libraries/luasocket.patch)
else()
    message(FATAL_ERROR "Patch executable not found! Cannot patch LuaSocket for Wii U.")
endif()

dkp_track_assets(${PROJECT_NAME}_cafe_content
    FOLDER shaders
    FILES
        color.gsh
        texture.gsh
)

dkp_track_assets(${PROJECT_NAME}_cafe_content
    FOLDER nogame
    FILES
        cartridge.png
        nogame.png
)

find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Freetype::Freetype bz2 png turbojpeg ddsparse
    vorbisfile vorbis
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/boot.cpp
    source/seeprom.cpp
    source/common/screen.cpp
    source/driver/audio/AudioBuffer.cpp
    source/driver/audio/DigitalSound.cpp
    source/driver/audio/SoundChannel.cpp
    source/driver/display/Framebuffer.cpp
    source/driver/display/GX2.cpp
    source/driver/display/Uniform.cpp
    source/driver/EventQueue.cpp
    source/modules/audio/Source.cpp
    source/modules/graphics/Graphics.cpp
    source/modules/graphics/Shader.cpp
    source/modules/graphics/ShaderStage.cpp
    source/modules/graphics/Texture.cpp
    source/modules/joystick/vpad/Joystick.cpp
    source/modules/joystick/kpad/Joystick.cpp
    source/modules/joystick/JoystickModule.cpp
    source/modules/keyboard/Keyboard.cpp
    source/modules/system/System.cpp
    source/modules/timer/Timer.cpp
    source/modules/window/Window.cpp
)

wut_create_rpx(${PROJECT_NAME})

wut_create_wuhb(${PROJECT_NAME}
    NAME       "${APP_TITLE}"
    CONTENT    ${PROJECT_NAME}_cafe_content
    SHORTNAME  "${APP_TITLE}"
    AUTHOR     ${APP_AUTHOR}
    ICON       icon$<$<CONFIG:Debug>:-dev>.png
)
