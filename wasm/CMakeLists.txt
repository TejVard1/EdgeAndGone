add_executable(cricket_core_wasm
    bindings.cpp
)

target_link_libraries(cricket_core_wasm PRIVATE cricket::core)

target_link_options(cricket_core_wasm
    PRIVATE
        --bind
        -sMODULARIZE=1
        -sEXPORT_ES6=1
        -sENVIRONMENT=web
        -sALLOW_MEMORY_GROWTH=1
        -sFILESYSTEM=0
)

set_target_properties(cricket_core_wasm PROPERTIES
    OUTPUT_NAME "cricket-core"
    SUFFIX ".js"
)