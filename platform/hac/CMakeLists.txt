dkp_add_asset_target(${PROJECT_NAME}_hac_romfs romfs)

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

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f -name \*.cpp | clip
target_sources(${PROJECT_NAME} PRIVATE
source/boot.cpp
source/common/screen.cpp
source/driver/audio/DigitalSound.cpp
source/driver/audio/MemoryPool.cpp
source/driver/display/deko3d/CIntrusiveTree.cpp
source/driver/display/deko3d/CMemPool.cpp
source/driver/display/Framebuffer.cpp
source/driver/display/deko3d.cpp
source/driver/EventQueue.cpp
source/modules/audio/Source.cpp
source/modules/graphics/Graphics.cpp
source/modules/joystick/Joystick.cpp
source/modules/joystick/JoystickModule.cpp
source/modules/keyboard/Keyboard.cpp
source/modules/system/System.cpp
source/modules/timer/Timer.cpp
source/modules/window/Window.cpp
)
