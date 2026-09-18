cmake_minimum_required(VERSION 3.24)

project(
    BoundaryCricket
    VERSION 0.1.0
    DESCRIPTION "Cross-platform multiplayer cricket game"
    LANGUAGES CXX
)

option(CRICKET_BUILD_SERVER "Build the authoritative server" ON)
option(CRICKET_BUILD_TESTS "Build C++ tests" ON)
option(CRICKET_BUILD_WASM "Build the WebAssembly adapter" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(core)

if(CRICKET_BUILD_SERVER)
    add_subdirectory(server)
endif()

if(CRICKET_BUILD_WASM)
    if(NOT EMSCRIPTEN)
        message(FATAL_ERROR "CRICKET_BUILD_WASM requires the Emscripten toolchain")
    endif()
    add_subdirectory(wasm)
endif()

if(CRICKET_BUILD_TESTS)
    enable_testing()
    add_subdirectory(core/tests)
endif()