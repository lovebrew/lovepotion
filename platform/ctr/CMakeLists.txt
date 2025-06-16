dkp_add_asset_target(${PROJECT_NAME}_ctr_romfs romfs)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

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
