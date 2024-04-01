cmake_minimum_required(VERSION 3.5)

option(GAMEPAD_ENABLE_STATIC "Static library (default: ON)" ON)
option(GAMEPAD_ENABLE_SHARED "Shared library (default: OFF)" OFF)
option(GAMEPAD_ENABLE_JSON "Provide interface to load and save with json11 (default: ON)" ON)
option(GAMEPAD_ENABLE_INSTALL "Register files for install target (default: ON)" ON)

project(libgamepad VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_library(gamepad OBJECT
    ./src/hook.cpp
    ./src/log.cpp
    ./src/binding.cpp
    ./src/binding-default.cpp
    ./src/json11.cpp
    ./src/device.cpp
)

set_property(TARGET gamepad PROPERTY POSITION_INDEPENDENT_CODE 1)

if (GAMEPAD_ENABLE_JSON)
    add_compile_definitions(GAMEPAD_ENABLE_JSON)
endif()

target_include_directories(gamepad
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (UNIX AND NOT APPLE)
    target_sources(gamepad PRIVATE
        ./src/linux/hook-linux.cpp
        ./src/linux/device-linux.cpp
        ./src/linux/device-linux.hpp
        ./src/linux/binding-linux.cpp
    )
elseif (APPLE)
    target_sources(gamepad PRIVATE
        ./src/darwin/hook-darwin.cpp
    )
elseif(WIN32)
    target_sources(gamepad PRIVATE
        ./src/windows/hook-xinput.cpp
        ./src/windows/hook-dinput.cpp
        ./src/windows/binding-dinput.cpp
        ./src/windows/binding-xinput.cpp
        ./src/windows/device-dinput.cpp
        ./src/windows/device-dinput.hpp
        ./src/windows/device-xinput.cpp
        ./src/windows/device-xinput.hpp
    )
    add_compile_definitions(UNICODE _UNICODE)
    target_link_libraries(gamepad
        dinput8
        dxguid
    )
endif()

if (GAMEPAD_ENABLE_SHARED)
    add_library(gamepad_shared SHARED $<TARGET_OBJECTS:gamepad>)
    set_target_properties(gamepad_shared PROPERTIES OUTPUT_NAME "gamepad")
    target_link_libraries(gamepad_shared PRIVATE
        ${CMAKE_DL_LIBS} # For UNIX-like systems
    )
    if (GAMEPAD_ENABLE_INSTALL)
        install(TARGETS gamepad_shared DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif()
endif()

if (GAMEPAD_ENABLE_STATIC)
    add_library(gamepad_static STATIC $<TARGET_OBJECTS:gamepad>)
    set_target_properties(gamepad_static PROPERTIES OUTPUT_NAME "gamepad")
    if (GAMEPAD_ENABLE_INSTALL)
        install(TARGETS gamepad_static DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif()
endif()

if (GAMEPAD_ENABLE_INSTALL)
    install(FILES ./include/libgamepad.hpp
        DESTINATION include)
    install(FILES ./include/gamepad/binding.hpp
        ./include/gamepad/binding-dinput.hpp
        ./include/gamepad/binding-xinput.hpp
        ./include/gamepad/binding-linux.hpp
        ./include/gamepad/config.h
        ./include/gamepad/device.hpp
        ./include/gamepad/hook.hpp
        ./include/gamepad/hook-dinput.hpp
        ./include/gamepad/hook-linux.hpp
        ./include/gamepad/hook-xinput.hpp
        ./include/gamepad/log.hpp
        DESTINATION include/gamepad)
endif()

if (UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(gamepad ${CMAKE_THREAD_LIBS_INIT})
endif()

