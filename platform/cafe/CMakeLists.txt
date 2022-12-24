# declare an asset target for the executable's content (optional)
dkp_add_asset_target(${PROJECT_NAME}_cafe_content content)

find_program(latte_assembler latte-assembler
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}
    REQUIRED
)

if (latte_assembler)
    execute_process(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        COMMAND ${latte_assembler} assemble --vsh=transform_vsh.asm --psh=color_fsh.asm primitive.gsh
    )
    dkp_resolve_file(primitive_gsh ${CMAKE_CURRENT_SOURCE_DIR}/shaders/primitive.gsh)

    execute_process(
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        COMMAND ${latte_assembler} assemble --vsh=transform_vsh.asm --psh=texture_fsh.asm texture.gsh
    )
    dkp_resolve_file(texture_gsh  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/texture.gsh)

    dkp_install_assets (${PROJECT_NAME}_cafe_content
        DESTINATION "shaders"
        TARGETS primitive_gsh texture_gsh
    )
else()
    message(FATAL_ERROR "To build shaders you need to a copy of latte-assembler from decaf-emu. CMake will now exit." )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/modules/graphics_ext.cpp
    source/modules/joystickmodule_ext.cpp
    source/modules/keyboard_ext.cpp
    source/modules/love_ext.cpp
    source/modules/system_ext.cpp
    source/modules/timer_ext.cpp
    source/modules/window_ext.cpp
    source/objects/joystick_ext.cpp
    source/objects/rasterizer_ext.cpp
    source/objects/source_ext.cpp
    source/utilities/driver/dsp_ext.cpp
    source/utilities/driver/hid_ext.cpp
    source/utilities/driver/renderer_ext.cpp
    source/utilities/haptics/sixaxis_ext.cpp
    source/utilities/haptics/vibration_ext.cpp
    source/utilities/threads/thread_ext.cpp
    source/utilities/wpad.cpp
)
