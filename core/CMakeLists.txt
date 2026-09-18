add_library(cricket_core STATIC
    src/match.cpp
)

add_library(cricket::core ALIAS cricket_core)

target_include_directories(cricket_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(cricket_core PUBLIC cxx_std_20)