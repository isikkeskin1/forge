cmake_minimum_required(VERSION 3.20)
project(forge VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(FORGE_BUILD_TESTS "Build Forge tests" ON)
option(FORGE_BUILD_BENCHMARKS "Build Forge microbenchmarks" OFF)

find_package(Threads REQUIRED)

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
    src/forge_pipeline.c
    src/forge_expr.c
    src/forge_mmap.c
    src/forge_pool.c
)

target_include_directories(forge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(forge PUBLIC Threads::Threads)

target_compile_options(forge PRIVATE
    $<$<CONFIG:Debug>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow>
)

add_executable(forge_cli src/main.c)
target_link_libraries(forge_cli PRIVATE forge)

if(FORGE_BUILD_TESTS)
    enable_testing()
    foreach(test_name IN ITEMS smoke csv column i64_parse kernel scan selection table group sort join pipeline expr mmap pool)
        add_executable(forge_${test_name}_test tests/test_${test_name}.c)
        target_link_libraries(forge_${test_name}_test PRIVATE forge)
        add_test(NAME forge_${test_name}_test COMMAND forge_${test_name}_test)
    endforeach()
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
    add_executable(forge_bench_pipeline bench/bench_pipeline.c)
    target_link_libraries(forge_bench_pipeline PRIVATE forge)
endif()
