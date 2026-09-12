cmake_minimum_required(VERSION 3.20)
project(forge VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(FORGE_BUILD_TESTS "Build Forge tests" ON)
option(FORGE_BUILD_BENCHMARKS "Build Forge microbenchmarks" OFF)

add_library(forge STATIC
    src/forge.c
    src/forge_buffer.c
    src/forge_csv.c
    src/forge_column.c
    src/forge_i64_parse.c
    src/forge_kernel.c
    src/forge_scan.c
    src/forge_selection.c
    src/forge_table.c
    src/forge_group.c
    src/forge_sort.c
    src/forge_join.c
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

    add_executable(forge_column_test tests/test_column.c)
    target_link_libraries(forge_column_test PRIVATE forge)
    add_test(NAME forge_column_test COMMAND forge_column_test)

    add_executable(forge_i64_parse_test tests/test_i64_parse.c)
    target_link_libraries(forge_i64_parse_test PRIVATE forge)
    add_test(NAME forge_i64_parse_test COMMAND forge_i64_parse_test)

    add_executable(forge_kernel_test tests/test_kernel.c)
    target_link_libraries(forge_kernel_test PRIVATE forge)
    add_test(NAME forge_kernel_test COMMAND forge_kernel_test)

    add_executable(forge_scan_test tests/test_scan.c)
    target_link_libraries(forge_scan_test PRIVATE forge)
    add_test(NAME forge_scan_test COMMAND forge_scan_test)

    add_executable(forge_selection_test tests/test_selection.c)
    target_link_libraries(forge_selection_test PRIVATE forge)
    add_test(NAME forge_selection_test COMMAND forge_selection_test)

    add_executable(forge_table_test tests/test_table.c)
    target_link_libraries(forge_table_test PRIVATE forge)
    add_test(NAME forge_table_test COMMAND forge_table_test)

    add_executable(forge_group_test tests/test_group.c)
    target_link_libraries(forge_group_test PRIVATE forge)
    add_test(NAME forge_group_test COMMAND forge_group_test)

    add_executable(forge_sort_test tests/test_sort.c)
    target_link_libraries(forge_sort_test PRIVATE forge)
    add_test(NAME forge_sort_test COMMAND forge_sort_test)

    add_executable(forge_join_test tests/test_join.c)
    target_link_libraries(forge_join_test PRIVATE forge)
    add_test(NAME forge_join_test COMMAND forge_join_test)
endif()

if(FORGE_BUILD_BENCHMARKS)
    add_executable(forge_bench_scan bench/bench_scan.c)
    target_link_libraries(forge_bench_scan PRIVATE forge)

    add_executable(forge_bench_group bench/bench_group.c)
    target_link_libraries(forge_bench_group PRIVATE forge)

    add_executable(forge_bench_sort bench/bench_sort.c)
    target_link_libraries(forge_bench_sort PRIVATE forge)

    add_executable(forge_bench_join bench/bench_join.c)
    target_link_libraries(forge_bench_join PRIVATE forge)
endif()
