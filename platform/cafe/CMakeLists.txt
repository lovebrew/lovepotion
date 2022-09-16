target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/modules/joystickmodule_ext.cpp
    source/modules/love_ext.cpp
    source/modules/system_ext.cpp
    source/modules/timer_ext.cpp
    source/objects/joystick_ext.cpp
    source/utilities/driver/hid_ext.cpp
    source/utilities/threads/thread_ext.cpp
    source/utilities/wpad.cpp
)
