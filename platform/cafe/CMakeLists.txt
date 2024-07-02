dkp_add_asset_target(${PROJECT_NAME}_cafe_content content)

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

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
source/boot.cpp
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
source/modules/graphics/Texture.cpp
source/modules/joystick/Joystick.cpp
source/modules/joystick/JoystickModule.cpp
source/modules/keyboard/Keyboard.cpp
source/modules/system/System.cpp
source/modules/timer/Timer.cpp
source/modules/window/Window.cpp
)
