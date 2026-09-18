add_executable(cricket_core_tests
    match_tests.cpp
)

target_link_libraries(cricket_core_tests PRIVATE cricket::core)

add_test(
    NAME cricket_core_tests
    COMMAND cricket_core_tests
)