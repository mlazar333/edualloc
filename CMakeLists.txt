cmake_minimum_required(VERSION 3.16)
project(edualloc VERSION 0.1.0 LANGUAGES C)

option(EA_BUILD_TESTS "Build unit tests" ON)
option(EA_BUILD_BENCHMARK "Build benchmark" ON)

add_library(edualloc src/edualloc.c)
add_library(edualloc::edualloc ALIAS edualloc)
target_include_directories(edualloc
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)
target_compile_features(edualloc PUBLIC c_std_99)

if(MSVC)
    target_compile_options(edualloc PRIVATE /W4)
else()
    target_compile_options(edualloc PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(EA_BUILD_TESTS)
    enable_testing()
    add_executable(edualloc_tests tests/test_edualloc.c)
    target_link_libraries(edualloc_tests PRIVATE edualloc)
    add_test(NAME edualloc_tests COMMAND edualloc_tests)
endif()

if(EA_BUILD_BENCHMARK)
    add_executable(edualloc_benchmark benchmarks/benchmark.c)
    target_link_libraries(edualloc_benchmark PRIVATE edualloc)
endif()

include(GNUInstallDirs)
install(TARGETS edualloc EXPORT eduallocTargets)
install(FILES include/edualloc.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
