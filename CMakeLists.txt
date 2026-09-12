cmake_minimum_required(VERSION 3.20)
project(forge VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(FORGE_BUILD_TESTS "Build Forge tests" ON)

add_library(forge STATIC
    src/forge.c
    src/forge_buffer.c
    src/forge_csv.c
)

target_include_directories(forge
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(forge PRIVATE
    $<$<CONFIG:Debug>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow>
)

add_executable(forge_cli
    src/main.c
)

target_link_libraries(forge_cli PRIVATE forge)

if(FORGE_BUILD_TESTS)
    enable_testing()

    add_executable(forge_smoke_test tests/test_smoke.c)
    target_link_libraries(forge_smoke_test PRIVATE forge)
    add_test(NAME forge_smoke_test COMMAND forge_smoke_test)

    add_executable(forge_csv_test tests/test_csv.c)
    target_link_libraries(forge_csv_test PRIVATE forge)
    add_test(NAME forge_csv_test COMMAND forge_csv_test)
endif()
