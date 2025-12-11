dkp_add_asset_target(${PROJECT_NAME}_hac_romfs romfs)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    __CONSOLE__="switch" __OS__="horizon"
    __RENDERER_NAME__="deko3d" __RENDERER_VERSION__="0.5.0"
    __RENDERER_VENDOR__="devkitPro" __RENDERER_DEVICE__="NVIDIA Tegra X1"
)

if (Patch_FOUND)
    message(STATUS "Patching LuaSocket for Switch")
    execute_process(COMMAND ${Patch_EXECUTABLE} -d ${LUASOCKET_COPY_DIR}/luasocket/libluasocket -p1 -N -i ${CMAKE_CURRENT_SOURCE_DIR}/libraries/luasocket.patch)
else()
    message(FATAL_ERROR "Patch executable not found! Cannot patch LuaSocket for Switch.")
endif()

nx_add_shader_program(transform_vsh "shaders/transform_vsh.glsl" "vert")
nx_add_shader_program(color_fsh     "shaders/color_fsh.glsl"     "frag")
nx_add_shader_program(texture_fsh   "shaders/texture_fsh.glsl"   "frag")
nx_add_shader_program(video_fsh     "shaders/video_fsh.glsl"     "frag")

dkp_install_assets (${PROJECT_NAME}_hac_romfs
    DESTINATION "shaders"
    TARGETS
        transform_vsh
        color_fsh
        texture_fsh
        video_fsh
)

dkp_track_assets(${PROJECT_NAME}_hac_romfs
    FOLDER "nogame"
    FILES
        cartridge.png
        nogame.png
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    deko3d$<$<CONFIG:Debug>:d>
    freetype bz2 png turbojpeg ddsparse
    vorbisidec
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f -name \*.cpp | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/boot.cpp
    source/common/screen.cpp
    source/driver/audio/DigitalSound.cpp
    source/driver/audio/DigitalSoundMemory.cpp
    source/driver/audio/MemoryPool.cpp
    source/driver/display/deko3d/CIntrusiveTree.cpp
    source/driver/display/deko3d/CMemPool.cpp
    source/driver/display/Framebuffer.cpp
    source/driver/display/deko.cpp
    source/driver/EventQueue.cpp
    source/modules/audio/RecordingDevice.cpp
    source/modules/font/SystemFont.cpp
    source/modules/graphics/Graphics.cpp
    source/modules/graphics/Shader.cpp
    source/modules/graphics/ShaderStage.cpp
    source/modules/graphics/StreamBuffer.cpp
    source/modules/graphics/Texture.cpp
    source/modules/joystick/Joystick.cpp
    source/modules/joystick/JoystickModule.cpp
    source/modules/keyboard/Keyboard.cpp
    source/modules/system/System.cpp
    source/modules/timer/Timer.cpp
    source/modules/window/Window.cpp
)

nx_generate_nacp(${PROJECT_NAME}.nacp
    NAME    "${APP_TITLE}"
    AUTHOR  "LÖVEBrew Team"
    VERSION ${PROJECT_VERSION}
)

nx_create_nro(${PROJECT_NAME}
    ICON  icon$<$<CONFIG:Debug>:-dev>.jpg
    NACP  ${PROJECT_NAME}.nacp
    ROMFS ${PROJECT_NAME}_hac_romfs
)
