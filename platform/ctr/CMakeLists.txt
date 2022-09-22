target_include_directories(${PROJECT_NAME} PRIVATE
    include
    libraries
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/modules/joystickmodule_ext.cpp
    source/modules/love_ext.cpp
    source/modules/system_ext.cpp
    source/modules/timer_ext.cpp
    source/modules/wrap_system_ext.cpp
    source/objects/joystick_ext.cpp
    source/runtime.cpp
    source/utilities/driver/hid_ext.cpp
    source/utilities/driver/ndsp.cpp
    source/utilities/haptics/sixaxis_ext.cpp
    source/utilities/threads/thread_ext.cpp
)