add_library(cricket_server_domain STATIC
    src/room.cpp
)

add_library(cricket::server_domain ALIAS cricket_server_domain)

target_include_directories(cricket_server_domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cricket_server_domain PUBLIC cricket::core)
target_compile_features(cricket_server_domain PUBLIC cxx_std_20)

add_executable(cricket_server
    src/main.cpp
)

target_link_libraries(cricket_server
    PRIVATE
        cricket::core
        cricket::server_domain
)