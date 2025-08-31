cmake_minimum_required(VERSION 3.13)

dkp_add_asset_target(${PROJECT_NAME}_ctr_romfs romfs)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    __CONSOLE__="3ds" __OS__="horizon"
    __RENDERER_NAME__="citro3d" __RENDERER_VERSION__="1.7.0"
    __RENDERER_VENDOR__="devkitPro" __RENDERER_DEVICE__="DMP PICA200"
)

if (Patch_FOUND)
    message(STATUS "Patching LuaSocket for 3DS")
    execute_process(COMMAND ${Patch_EXECUTABLE} -d ${LUASOCKET_COPY_DIR}/luasocket/libluasocket -p1 -N -i ${CMAKE_CURRENT_SOURCE_DIR}/libraries/luasocket.patch)
else()
    message(FATAL_ERROR "Patch executable not found! Cannot patch LuaSocket for 3DS.")
endif()

ctr_add_graphics_target(cartridge
    IMAGE
    OPTIONS -f rgba8888 -z auto
    INPUTS "assets/nogame/cartridge.png"
)

ctr_add_graphics_target(nogame
    IMAGE
    OPTIONS -f rgba8888 -z auto
    INPUTS "assets/nogame/nogame.png"
)

dkp_install_assets(${PROJECT_NAME}_ctr_romfs
    DESTINATION "nogame"
    TARGETS
        cartridge
        nogame
)

ctr_add_shader_library(main_v_pica
    "assets/shaders/main.v.pica"
)

dkp_install_assets(${PROJECT_NAME}_ctr_romfs
    DESTINATION "shaders"
    TARGETS
        main_v_pica
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    citro3d$<$<CONFIG:Debug>:d>
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
    source/driver/display/Framebuffer.cpp
    source/driver/display/citro3d.cpp
    source/driver/EventQueue.cpp
    source/modules/audio/Source.cpp
    source/modules/font/BCFNTRasterizer.cpp
    source/modules/font/Font.cpp
    source/modules/graphics/Graphics.cpp
    source/modules/graphics/Shader.cpp
    source/modules/graphics/Texture.cpp
    source/modules/graphics/Font.cpp
    source/modules/graphics/ShaderStage.cpp
    source/modules/joystick/Joystick.cpp
    source/modules/joystick/JoystickModule.cpp
    source/modules/keyboard/Keyboard.cpp
    source/modules/system/System.cpp
    source/modules/timer/Timer.cpp
    source/modules/window/Window.cpp
)

ctr_generate_smdh(${PROJECT_NAME}.smdh
    NAME        "${APP_TITLE}"
    DESCRIPTION "LÖVE for 3DS • ${PROJECT_VERSION}"
    AUTHOR      "LÖVEBrew Team"
    ICON        icon$<$<CONFIG:Debug>:-dev>.png
)

ctr_create_3dsx(${PROJECT_NAME}
    SMDH  ${PROJECT_NAME}.smdh
    ROMFS ${PROJECT_NAME}_ctr_romfs
)
